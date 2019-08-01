/*
  Blinkin' Rubber Duckies

  Uses UNO + Ethernet + Proto shield:

 * Subscribe to DiscoDucks topics at MQTT broker mosquitto
   + 'effect' Use tiny interpreter for effect strings.
   + 'speed'  Use tiny interpreter for effect strings.

 * Publish two topics to MQTT broker mosquitto
   + 'keypress'  Key press symbols
   + 'potivalue' Potentiometer value (for sleep time)

 * MQTT - http://knolleary.net/arduino-client-for-mqtt/

 * My IOT demo network: 192.168.107.0/24
 
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

// 2 simple effects
prog_char eff_OK[]  PROGMEM =		// alternate blinking:  left / right
	            "Az3 Za3" ;                  
prog_char eff_SOS[] PROGMEM =		// morse code: SOS
		    "AZ2az2 AZ2az2 AZ2az2 4 AZ4az2 AZ4az2 AZ4az2 4 AZ2az2 AZ2az2 AZ2az2 9" ;    

char* Effects[] PROGMEM = {
        eff_OK, eff_SOS
};

// Effect buffer
char Buffer[128];
volatile char Change = 0;

// LED state variables
byte L, R;

/****************************************/

// values for MQTT client
#define CLIENTID  "ArduinoDiscoDucks"
#define TOPIC1 "discoducks/effect"
#define TOPIC2 "discoducks/speed"
#define TOPIC3 "discoducks/keypress"
#define TOPIC4 "discoducks/potivalue"

// Our IP is fixed
byte ip[4] = { 192, 168, 107, 93 };
// Connect to MQTT on IOT network (HASS OS)
byte server[4] = { 192, 168, 107, 10 };
// Some MAC 
byte mac[6] = {'m', 's', 'd', 0x42, 0x42, 0x03} ;

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

// copy initial effect to global Effect buffer
void initial_effect(int pos) {
  strcpy_P(Buffer, (char*)pgm_read_word(&(Effects[pos])));
}

// Called by MQTT callback function:
//   copy effect string from MQTT buffer to global Effect buffer
void set_Effect(char *buffer, unsigned int length) {
  if (length > 127) length = 127;
  strncpy((char *)Buffer, buffer, length);
  Buffer[length] = 0;
  Serial.println((char *) Buffer);
  // set global Change flag
  Change = 1;
}

/****************************************/

#define UNIT  50      // 50 ms
#define MULT  10      // max delay
int Sleep;
int pSleep = 1;

// use voltage at potentiometer to determine sleep time
void check_poti(int pin) {
  int poti = analogRead(pin);
  float value = (poti * MULT) / 1023.0;
  Sleep = (1.0 + value) * UNIT;
  // sleep values between 50 and 550: (1 .. 1+10) * 50
  Serial.println(Sleep);
}

// Called by MQTT callback function:
//   get sleep time from from MQTT buffer
void set_Speed(char *buffer, unsigned int length) {
  String number = String(buffer);
  int val = number.toInt();
  Sleep = val;
  Serial.println(val);
}

// send topic and value to MQTT broker
void publish_poti() {
  char topic[30] = TOPIC4 ;
  char value[30];

  sprintf(value, "%3d", Sleep);
  arduinoClient.publish(topic, value) ;
}

/****************************************/

// key press codes
#define KS_Undef   0
#define KS_None    1
#define KS_Red     2
#define KS_Yellow  3
#define KS_Both    4

// last key press
int pKey = KS_Undef;

// use voltage levels to determine pressed keys
int check_keys(int pin) {
  int level = analogRead(pin);
  if (abs(level-380) < 10)      return KS_Red;
  if (abs(level-510) < 10)      return KS_Both;
  if (abs(level-610) < 10)      return KS_None;
  if (level > 1010)             return KS_Yellow;
  return KS_Undef;
}

// send topic and value to MQTT broker
void publish_keys(int keysym) {
  char topic[30] = TOPIC3 ;
  char value[30];

  sprintf(value, "%1d", keysym);
  arduinoClient.publish(topic, value) ;
}

/****************************************/

// turn the LEDs on/off (HIGH/LOW is the voltage level)
void apply_state(void) {
  digitalWrite(l1, (L &  1 ? HIGH : LOW));      digitalWrite(r1, (R &  1 ? HIGH : LOW));
  digitalWrite(l2, (L &  2 ? HIGH : LOW));      digitalWrite(r2, (R &  2 ? HIGH : LOW));
  digitalWrite(l3, (L &  4 ? HIGH : LOW));      digitalWrite(r3, (R &  4 ? HIGH : LOW));
  digitalWrite(l4, (L &  8 ? HIGH : LOW));      digitalWrite(r4, (R &  8 ? HIGH : LOW));
  digitalWrite(l5, (L & 16 ? HIGH : LOW));      digitalWrite(r5, (R & 16 ? HIGH : LOW));
}

// global Wait time
int Wait = 0;

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
    // store wait time for next usage
    Wait = wait;
  } else
  if (code == '*') {
    // flash all LEDs (invert state)
    L ^= 31;
    R ^= 31;
    change = 1;
  } else
  if (code == '#') {
    // use stored wait time
    wait = Wait;
  } else
  if (code == '$') {
    // send "end-of-effect"
    publish_eoe();
  } else
  if (code == '/') {
    // cut init sequence
    return 1;
  }

  if (change)	apply_state();
  if (wait > 0)	delay(wait);
  return 0;
}

// send topic and value to MQTT broker
void publish_eoe() {
  char topic[30] = TOPIC1 ;
  char value[30] = "end-of-effect";

  arduinoClient.publish(topic, value) ;
}

/****************************************/

// the setup routine runs once when you press reset:
void setup() {                
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  // initialize all required IO pins as an output.
  pinMode(l1, OUTPUT);    pinMode(r1, OUTPUT);
  pinMode(l2, OUTPUT);    pinMode(r2, OUTPUT);
  pinMode(l3, OUTPUT);    pinMode(r3, OUTPUT);
  pinMode(l4, OUTPUT);    pinMode(r4, OUTPUT);
  pinMode(l5, OUTPUT);    pinMode(r5, OUTPUT);

  // switch all LEDs off
  L = 0;  R = 0;
  apply_state();

  // Start Ethernet client -- use fixed IP
  Ethernet.begin(mac, ip);

  // Connect to the MQTT server
  Serial.println("start MQTT") ;
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
    initial_effect(1) ;
  }
  else {
    Serial.println(F("Connected to MQTT Server..."));
    initial_effect(0) ;
  }

  arduinoClient.subscribe(TOPIC1) ;
  arduinoClient.subscribe(TOPIC2) ;
}

/****************************************/

/* Tiny interpreter for effect strings.

  Endless loop:
    1)	read analog value from potentiometer
	and compute delay value (= sleep time)
	and publish to mqtt broker
    2)  read analog value from push buttons
	and publish to mqtt broker
    3)	while not (end-of-string or new-effect-string) :
    4)	    get next char from effect string
    5)	    if led code:
		turn on/off corresponding LED pin(s)
    6)	    if wait code:
		sleep N * delay value
    7)	    if special effect:
		act accordingly
    8)	    if MQTT has delivered a new effect string
		terminate this effect loop
		and start over with new effect
  Repeat.
 */
 
// the loop routine runs over and over again forever:
void loop(void) {

  // check potentiometer on analog pin A5
  check_poti(A5);
  if (Sleep != pSleep) {
    // publish poti state via MQTT
    publish_poti();
    pSleep = Sleep;
  }

  // check key press on analog pin A4
  int curr = check_keys(A4);
  if (curr != pKey) {
    // publish key state via MQTT
    publish_keys(curr);
    pKey = curr;
  }

  int len = strlen((char *)Buffer);
  for (int i=0; i<len; i++) {
    // keep MQTT connection alive
    arduinoClient.loop();

    if (eval_code(Buffer[i])) {
      // true from eval_code: delete init sequence
      for (int k=0; k<=i; k++)
        Buffer[k] = ' ';
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

  // publish "end-of-effect" via MQTT	
  publish_eoe();
}

