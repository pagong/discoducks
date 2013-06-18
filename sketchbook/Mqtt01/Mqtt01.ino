/*
  Blinkin' Rubber Duckies

  Uses UNO + Ethernet + Proto shield:

 * Subscribe to DiscoDucks topics at MQTT broker mosquitto
   + 'effect' Use tiny interpreter for effect strings.
   + 'speed'  Use tiny interpreter for effect strings.

 * MQTT - http://knolleary.net/arduino-client-for-mqtt/

  This example code is in the public domain.
 */
 
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// Give names to IO pins
#define l1   8		// digital 8
#define l2   9		// digital 9
#define l3   A2		// analog 2
#define l4   A1		// analog 1
#define l5   A0		// analog 0
#define r1   2		// digital 2
#define r2   3		// digital 3
#define r3   7		// digital 7
#define r4   5		// digital 5
#define r5   6		// digital 6

// Effect buffer: Knight Rider
volatile char Effect[128] = "F2E2/ Df2Ce2Bd2 Yc2Xb2Wy2Vx2Uw2 3 Wu2Xv2Yw2 Bx2Cy2Db2Ec2Fd2 3";
volatile char Change = 0;

// LED state variables
byte L, R;

/****************************************/

#define CLIENTID  "ArduinoDiscoDucks"
#define TOPIC1 "discoducks/effect"
#define TOPIC2 "discoducks/speed"

// Some MAC 
byte mac [] = {'m', 's', 'd', 0x42, 0x42, 0x03} ;
// Our IP comes from DHCP
//byte ip[4] = { 0, 0, 0, 0 };
byte ip[4] = { 192, 168, 1, 77 };
// Connect to spider.v3.msd
byte server [4] = { 192, 168, 4, 1 };
//byte server [4] = { 192, 168, 1, 101 };

// Our Ethernet client
EthernetClient ethClient;
// Our MQTT client
PubSubClient arduinoClient(server, 1883, callback, ethClient) ;

// Handle message from mosquitto
void callback(char *topic, byte *payload, unsigned int length) {
  Serial.println(topic);
  if (topic[11] == 'e')	set_Effect((char *)payload, length);
  if (topic[11] == 's')	set_Speed( (char *)payload, length);
}

/****************************************/

// the setup routine runs once when you press reset:
void setup() {                
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  // initialize all required IO pins as an output.
  pinMode(l1, OUTPUT);	pinMode(r1, OUTPUT);
  pinMode(l2, OUTPUT);  pinMode(r2, OUTPUT);
  pinMode(l3, OUTPUT);  pinMode(r3, OUTPUT);
  pinMode(l4, OUTPUT);  pinMode(r4, OUTPUT);
  pinMode(l5, OUTPUT);  pinMode(r5, OUTPUT);

  // switch all LEDs off
  L = 0;  R = 0;
  apply_state();

  // Start Ethernet client -- get IP via DHCP
//  Ethernet.begin(mac);
  Ethernet.begin(mac, ip);
  // Connect to the MQTT server
  beginConnection() ;
}

//Initialise MQTT connection
void beginConnection() {
  int connRC = arduinoClient.connect(CLIENTID) ;
  if (!connRC) {
    Serial.println(connRC) ;
    Serial.println(F("Could not connect to MQTT Server"));
    Serial.println(F("Please reset the arduino to try again"));
    delay(100);
    exit(-1);
  }
  else {
    Serial.println(F("Connected to MQTT Server..."));
  }

  arduinoClient.subscribe(TOPIC1) ;
  arduinoClient.subscribe(TOPIC2) ;
}

/****************************************/

// copy effect string from MQTT buffer to global Effect buffer
void set_Effect(char *buffer, unsigned int length) {
  if (length > 127) length = 127;
  strncpy((char *)Effect, buffer, length);
  Effect[length] = 0;
  // set global Change flag
  Change = 1;
  Serial.println((char *) Effect);
}

/****************************************/

#define UNIT  50      // 50 ms
#define MULT  10      // max delay
int sleep;

// use voltage at potentiometer to determine sleep time
void check_poti(int pin) {
  int poti = analogRead(pin);
  float value = (poti * MULT) / 1023.0;
  sleep = (1.0 + value) * UNIT;
  Serial.println(sleep);
}

void set_Speed(char *buffer, unsigned int length) {
  String number = String(buffer);
  int val = number.toInt();
  Serial.println(val);
}

/****************************************/

// turn the LEDs on/off (HIGH/LOW is the voltage level)
void apply_state(void) {
  digitalWrite(l1, (L &  1 ? HIGH : LOW));
  digitalWrite(l2, (L &  2 ? HIGH : LOW));
  digitalWrite(l3, (L &  4 ? HIGH : LOW));
  digitalWrite(l4, (L &  8 ? HIGH : LOW));
  digitalWrite(l5, (L & 16 ? HIGH : LOW));
  digitalWrite(r1, (R &  1 ? HIGH : LOW));
  digitalWrite(r2, (R &  2 ? HIGH : LOW));
  digitalWrite(r3, (R &  4 ? HIGH : LOW));
  digitalWrite(r4, (R &  8 ? HIGH : LOW));
  digitalWrite(r5, (R & 16 ? HIGH : LOW));
}

int eval_code(char code) {
  int change = 0;
  int wait = 0;

  if (('A' <= code) && (code <= 'Z')) {
    // turn LEDs on
    change = 1;
    switch (code) {
      case 'A':	L  = 31;	break;
      case 'B':	L |=  1;	break;
      case 'C':	L |=  2;	break;
      case 'D':	L |=  4;	break;
      case 'E':	L |=  8;	break;
      case 'F':	L |= 16;	break;
      case 'Z':	R  = 31;	break;
      case 'Y':	R |=  1;	break;
      case 'X':	R |=  2;	break;
      case 'W':	R |=  4;	break;
      case 'V':	R |=  8;	break;
      case 'U':	R |= 16;	break;
      default:	change = 0;	break;
    }
  } else
  if (('a' <= code) && (code <= 'z')) {
    // turn LEDs off
    change = 1;
    switch (code) {
      case 'a':	L  =  0;	break;
      case 'b':	L &= 30;	break;
      case 'c':	L &= 29;	break;
      case 'd':	L &= 27;	break;
      case 'e':	L &= 23;	break;
      case 'f':	L &= 15;	break;
      case 'z':	R  =  0;	break;
      case 'y':	R &= 30;	break;
      case 'x':	R &= 29;	break;
      case 'w':	R &= 27;	break;
      case 'v':	R &= 23;	break;
      case 'u':	R &= 15;	break;
      default:	change = 0;	break;
    }
  } else
  if (('0' <= code) && (code <= '9')) {
    // compute wait time
    wait = (code == '0' ? sleep / 2 : (code - '0') * sleep);
  } else
  if (code == '*') {
    // flash all LEDs (invert state)
    L ^= 31;
    R ^= 31;
    change = 1;
  } else
  if (code == '/') {
    // cut init sequence
    return 1;
  }

  if (change)	apply_state();
  if (wait > 0)	delay(wait);
  return 0;
}

/****************************************/

/* Tiny interpreter for effect strings.

  Endless loop:
    1)	read analog value from potentiometer
	and compute delay value (= sleep time)
    2)	while not (end-of-string or new-effect-string) :
    3)	    get next char from effect string
    4)	    if led code:
		turn on/off corresponding LED pin(s)
    5)	    if wait code:
		sleep N * delay value
    6)	    if special effect:
		act accordingly
    7)	    if MQTT has delivered a new effect string
		terminate this effect loop
  Repeat.
 */
 
// the loop routine runs over and over again forever:
void loop(void) {
  // check potentiometer on analog pin A5
  check_poti(A5);

  int len = strlen((char *)Effect);
  for (int i=0; i<len; i++) {
    // keep MQTT connection alive
    arduinoClient.loop();

    if (eval_code(Effect[i])) {
      // true from eval_code: delete init sequence
      for (int k=0; k<=i; k++)
        Effect[k] = ' ';
    }

    // terminate loop if we've recvd a new effect string via MQTT
    if (Change) {
      i = len;
      Change = 0;
      // switch all LEDs off
      L = 0;  R = 0;
      apply_state();
    }
  }
}

