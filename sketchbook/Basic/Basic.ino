/*
  Blinkin' Rubber Duckies

  Basic test routine for the proto shield.
  Endless loop:
    1)	read analog value from potentiometer
	and compute delay value (= sleep time)
    2)	turn on 5 ducks on the left side
	(left to middle - use delay between each step)
    3)	wait 1 second
    4)	turn on 5 ducks on the right side
	(middle to right - use delay between each step)
    5)	wait 1 second
    6)	turn off 5 ducks on the left side
	(left to middle - use delay between each step)
    7)	wait 1 second
    8)	turn off 5 ducks on the right side
	(middle to right - use delay between each step)
    9)	wait 1 second
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
static int L, R;

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

void led_on(int pin) {
  digitalWrite(pin, HIGH);   // turn the LED on (HIGH is the voltage level)
}

void led_off(int pin) {
  digitalWrite(pin, LOW);   // turn the LED off (LOW is the voltage level)
}

void left_on(int sleep) {
  led_on(l5);	delay(sleep);
  led_on(l4);	delay(sleep);
  led_on(l3);	delay(sleep);
  led_on(l2);	delay(sleep);
  led_on(l1);	delay(sleep);
}

void left_off(int sleep) {
  led_off(l5);	delay(sleep);
  led_off(l4);	delay(sleep);
  led_off(l3);	delay(sleep);
  led_off(l2);	delay(sleep);
  led_off(l1);	delay(sleep);
}

void right_on(int sleep) {
  led_on(r1);	delay(sleep);
  led_on(r2);	delay(sleep);
  led_on(r3);	delay(sleep);
  led_on(r4);	delay(sleep);
  led_on(r5);	delay(sleep);
}

void right_off(int sleep) {
  led_off(r1);	delay(sleep);
  led_off(r2);	delay(sleep);
  led_off(r3);	delay(sleep);
  led_off(r4);	delay(sleep);
  led_off(r5);	delay(sleep);
}

#define UNIT 100      // 100 ms
#define MULT  10      // max delay

// the loop routine runs over and over again forever:
void loop() {

  int poti = analogRead(A5);
  float value = (poti * MULT) / 1023.0;
  int sleep = (1.0 + value) * UNIT;
  Serial.println(sleep);

  left_on(sleep);
  delay(1000);               // wait for a second
  right_on(sleep);
  delay(1000);               // wait for a second
  left_off(sleep);
  delay(1000);               // wait for a second
  right_off(sleep);
  delay(1000);               // wait for a second
}

