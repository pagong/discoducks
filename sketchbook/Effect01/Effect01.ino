/*
  Blinkin' Rubber Duckies

  Basic test routine for the proto shield.
  Use tiny interpreter for effect strings.

  Endless loop:
    1)	read analog value from potentiometer
	and compute delay value (= sleep time)
    2)	while not end-of-string :
    3)	    get next char from effect string
    4)	    if led code:
		turn on/off corresponding LED pin(s)
    5)	    if wait code:
		sleep N * delay value
    6)	    if special effect:
		act accordingly
  Repeat.
 
  This example code is in the public domain.
 */
 
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

// state variables
int L, R;
int sleep;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize all required IO pins as an output.
  pinMode(l1, OUTPUT);     pinMode(l2, OUTPUT);     
  pinMode(l3, OUTPUT);     pinMode(l4, OUTPUT);     
  pinMode(l5, OUTPUT);     L = 0;
  pinMode(r1, OUTPUT);     pinMode(r2, OUTPUT);     
  pinMode(r3, OUTPUT);     pinMode(r4, OUTPUT);     
  pinMode(r5, OUTPUT);     R = 0;
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

// turn the LEDs on/off (HIGH/LOW is the voltage level)
void apply_state() {
  digitalWrite(l1, (L &  1 ? HIGH : LOW));
  digitalWrite(r1, (R &  1 ? HIGH : LOW));
  digitalWrite(l2, (L &  2 ? HIGH : LOW));
  digitalWrite(r2, (R &  2 ? HIGH : LOW));
  digitalWrite(l3, (L &  4 ? HIGH : LOW));
  digitalWrite(r3, (R &  4 ? HIGH : LOW));
  digitalWrite(l4, (L &  8 ? HIGH : LOW));
  digitalWrite(r4, (R &  8 ? HIGH : LOW));
  digitalWrite(l5, (L & 16 ? HIGH : LOW));
  digitalWrite(r5, (R & 16 ? HIGH : LOW));
}

void eval_code(char code) {
  int change = 0;
  int wait = 0;

  if (('A' <= code) && (code <= 'Z')) {
    // turn LEDs on
    if (code == 'A')	L  = 31;
    if (code == 'B')	L |=  1;
    if (code == 'C')	L |=  2;
    if (code == 'D')	L |=  4;
    if (code == 'E')	L |=  8;
    if (code == 'F')	L |= 16;
    if (code == 'Z')	R  = 31;
    if (code == 'Y')	R |=  1;
    if (code == 'X')	R |=  2;
    if (code == 'W')	R |=  4;
    if (code == 'V')	R |=  8;
    if (code == 'U')	R |= 16;
    change = 1;
  } else
  if (('a' <= code) && (code <= 'z')) {
    // turn LEDs off
    if (code == 'a')	L  =  0;
    if (code == 'b')	L &= 30;
    if (code == 'c')	L &= 29;
    if (code == 'd')	L &= 27;
    if (code == 'e')	L &= 23;
    if (code == 'f')	L &= 15;
    if (code == 'z')	R  =  0;
    if (code == 'y')	R &= 30;
    if (code == 'x')	R &= 29;
    if (code == 'w')	R &= 27;
    if (code == 'v')	R &= 23;
    if (code == 'u')	R &= 15;
    change = 1;
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
  }

  if (change)	apply_state();
  if (wait > 0)	delay(wait);
}

static char Effect[] = "BY2CX2DW2EV2FU2 3*0*0*0*0*0*5 by2cx2dw2ev2fu2 3*0*0*0*0*0*5";

#define UNIT  50      // 50 ms
#define MULT  10      // max delay

// the loop routine runs over and over again forever:
void loop() {

  int poti = analogRead(A5);
  float value = (poti * MULT) / 1023.0;
  sleep = (1.0 + value) * UNIT;
  Serial.println(sleep);

  int len = strlen(Effect);
  for (int i=0; i<len; i++) {
    eval_code(Effect[i]);
  }
}

