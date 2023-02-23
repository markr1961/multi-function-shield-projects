#define LED_OFF HIGH
#define LED_ON  LOW
#define BUTTON_DOWN true
#define BUTTON_RELEASED false

#define POT1    A0
#define BUTTON1 A1
#define BUTTON2 A2
#define BUTTON3 A3

#define LED1 13
#define LED2 12
#define LED3 11
#define LED4 10

#include <Servo.h>
Servo myservo;  // create servo object to control a servo
#define MIDDLE 90
#define STEP 5
#define LOWER_LIMIT 0
#define UPPER_LIMIT 180

int pos = MIDDLE;    // variable to store the servo position

void setup()
{
  // initialize the digital pin as an output.
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  digitalWrite(LED1, LED_OFF);
  digitalWrite(LED2, LED_OFF);
  digitalWrite(LED3, LED_OFF);
  analogWrite(LED4, pos);
  Serial.begin(9600);

  myservo.attach(9);  // attaches the servo on pin 9 to the servo object

  InitDisplay();
}

void loop()
{
  static bool button1 = BUTTON_RELEASED;
  static bool button2 = BUTTON_RELEASED;
  static bool button3 = BUTTON_RELEASED;
  static int lastPos = -1;

  if (!digitalRead(BUTTON1)) {
    digitalWrite(LED1, LED_ON);
    button1 = BUTTON_DOWN;
  }
  else if ((button1 == BUTTON_DOWN) && digitalRead(BUTTON1)){
    pos -= STEP;
    if (pos < LOWER_LIMIT)
      pos = LOWER_LIMIT;
    digitalWrite(LED1, LED_OFF);
    button1 = BUTTON_RELEASED;
  }

  if (!digitalRead(BUTTON2)) {
    digitalWrite(LED2, LED_ON);
    button2 = BUTTON_DOWN;
  }
  else if ((button2 == BUTTON_DOWN) && digitalRead(BUTTON2)){
    pos = MIDDLE;
    digitalWrite(LED2, LED_OFF);
    button2 = BUTTON_RELEASED;
  }

  if (!digitalRead(BUTTON3)) {
    digitalWrite(LED3, LED_ON);
    button3 = BUTTON_DOWN;
  }
  else if ((button3 == BUTTON_DOWN) && digitalRead(BUTTON3)){
    pos += STEP;
    if (pos > UPPER_LIMIT)
      pos = UPPER_LIMIT;
    digitalWrite(LED3, LED_OFF);
    button3 = BUTTON_RELEASED;
  }

  displayValue(pos);
  myservo.write(pos);              // tell servo to go to position in variable 'pos'
  analogWrite(LED4, UPPER_LIMIT - pos);
  if (pos != lastPos) {
  Serial.println(pos,DEC);
  lastPos = pos;
  }

}

/* Define shift register pins used for seven segment display */
#define LATCH_DIO 4
#define CLK_DIO   7
#define DATA_DIO  8

void InitDisplay(void)
{
  /* Set DIO pins to outputs */
  pinMode(LATCH_DIO, OUTPUT);
  pinMode(CLK_DIO, OUTPUT);
  pinMode(DATA_DIO, OUTPUT);
}
/* Segment byte maps for numbers 0 to 9 */
const byte SEGMENT_MAP[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0X80, 0X90};
/* Byte maps to select digit 1 to 4 */
const byte SEGMENT_SELECT[] = {0xF1, 0xF2, 0xF4, 0xF8};

///
// @brief write a 0 to 9 value to one of 4 digits
// @param digit  the digit to be driven (0-3)
// @param value  the value to be displayed on that digit
/** assumes there are (2) 74595 8-bit shift registers connected in series.
 *  the first drives the anode of one of four digits, the 2nd is connected
 *  to the shift out of the 1st and drives the 7 segments of all four digits.
 *  the data for the segments is shifted in first, followed by the digit.
 *  only one digit can be active at a time, so this function must be called
 *  repeatedly and regularly to light all 4 segments
 */
void WriteNumberToSegment(byte Digit, byte Value)
{
  // set the latch pin low
  digitalWrite(LATCH_DIO, LOW);
  // shift in the 7 segment value
  shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, SEGMENT_MAP[Value]);
  // shift in the anode driver
  shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, SEGMENT_SELECT[Digit] );
  // latch it
  digitalWrite(LATCH_DIO, HIGH);
}

void displayValue(int value)
{
  static byte blah = 0;
    /* Update the display with the current counter value */
  switch (blah)
  {
    case 0: // MSB
      WriteNumberToSegment(0 , value / 1000);
      blah++;
      break;

    case 1:
      WriteNumberToSegment(1 , (value / 100) % 10);
      blah++;
      break;

    case 2:
      WriteNumberToSegment(2 , (value / 10) % 10);
      blah++;
      break;

    default:
    case 3: // LSB
      WriteNumberToSegment(3 , value % 10);
      blah = 0;
      break;
  }
}


