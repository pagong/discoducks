/*
  Blinkin' Rubber Duckies

  Basic test routine for the proto shield.
  Use tiny interpreter for an array of effect strings.
  Use flash memory for effect strings.

  Endless loop:
    1)	read analog value from potentiometer
	and compute delay value (= sleep time)
    2)  read analog value from push buttons;
	if red button: use next effect string
    3)	while not end-of-string :
    4)	    get next char from effect string
    5)	    if led code:
		turn on/off corresponding LED pin(s)
    6)	    if wait code:
		sleep N * delay value
    7)	    if special effect:
		act accordingly
  Repeat.
 
  This example code is in the public domain.
 */
 
const char eff01[] PROGMEM = "az1/ B#Y#C#X#D#W#E#V#F#U# b#y#c#x#d#w#e#v#f#u#";     // roll in + roll out (right)
const char eff02[] PROGMEM = "az1/ W#D#X#C#Y#B#U#F#V#E# w#d#x#c#y#b#u#f#v#e#";     // roll in + roll out (left)
const char eff03[] PROGMEM = "az1/ AZ#az#";                                        // blink all ducks
const char eff04[] PROGMEM = "az1/ Az#Za#";                                        // alternate blinking
char const eff05[] PROGMEM = "az1/ BW#bwEX#exVC#vcFX#fxUD#ud";                     // spin right
char const eff06[] PROGMEM = "az1/ WB#wbDU#duXF#xfCV#cvYE#ye";                     // spin left
char const eff07[] PROGMEM = "az1/ Bf#Yu#Cb#Xy#Dc#Wx#Ed#Vw#Fe#Uv#";                // wandering sector right
char const eff08[] PROGMEM = "az1/ Wv#De#Xw#Cd#Yx#Bc#Uy#Fb#Vu#Ef#";                // wandering sector left

int num = 8;
const char* const Effects[] PROGMEM = {
	eff01, eff02, eff03, eff04, eff05, eff06, eff07, eff08
};

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

// LED state variables
byte L, R;

/******************************************************************/

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

/******************************************************************/

#define KS_Undef   0
#define KS_None    1
#define KS_Red     2
#define KS_Yellow  3
#define KS_Both    4

// use voltage levels to determine pressed keys
int check_keys(int pin) {
  int level = analogRead(pin);
  if (abs(level-380) < 10)	return KS_Red;
  if (abs(level-510) < 10)	return KS_Both;
  if (abs(level-610) < 10)	return KS_None;
  if (level > 1010)		return KS_Yellow;
  return KS_Undef;
}

/******************************************************************/

// turn the LEDs on/off (HIGH/LOW is the voltage level)
void apply_state() {
  digitalWrite(l1, (L &  1 ? HIGH : LOW));	digitalWrite(r1, (R &  1 ? HIGH : LOW));
  digitalWrite(l2, (L &  2 ? HIGH : LOW));	digitalWrite(r2, (R &  2 ? HIGH : LOW));
  digitalWrite(l3, (L &  4 ? HIGH : LOW));	digitalWrite(r3, (R &  4 ? HIGH : LOW));
  digitalWrite(l4, (L &  8 ? HIGH : LOW));	digitalWrite(r4, (R &  8 ? HIGH : LOW));
  digitalWrite(l5, (L & 16 ? HIGH : LOW));	digitalWrite(r5, (R & 16 ? HIGH : LOW));
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
  if (code == '/') {
    // cut init sequence
    return 1;
  }

  if (change)	apply_state();
  if (wait > 0)	delay(wait);
  return 0;
}

/******************************************************************/

// key press codes
int prev = KS_Undef;
// effect buffer
char buffer[128];

void change_effect(int pos) {
  L = 0; R = 0;
  apply_state();
  strcpy_P(buffer, (char*)pgm_read_word(&(Effects[pos])));
}

// the setup routine runs once when 'reboot':
void setup() {                
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // initialize all required IO pins as an output.
  pinMode(l1, OUTPUT);	pinMode(r1, OUTPUT);
  pinMode(l2, OUTPUT);	pinMode(r2, OUTPUT);     
  pinMode(l3, OUTPUT);	pinMode(r3, OUTPUT);
  pinMode(l4, OUTPUT);	pinMode(r4, OUTPUT);     
  pinMode(l5, OUTPUT);	pinMode(r5, OUTPUT); 
  change_effect(0);
}

// the loop routine runs over and over again forever:
void loop() {
  static int pos = 0;

  // check potentiometer on analog pin A5
  check_poti(A5);

  // check key press on analog pin A4
  int curr = check_keys(A4);
  if (curr != prev) {
    if (curr == KS_Red) {		// red key pressed
      // use next effect string
      if (++pos == num) pos = 0;
      change_effect(pos);
    }
    prev=curr;
  }
  
  int len = strlen(buffer);
  for (int i=0; i<len; i++) {
    if (eval_code(buffer[i])) {
      // true from eval_code: delete init sequence
      for (int k=0; k<=i; k++)
        buffer[k] = ' ';
    }
  }
}
