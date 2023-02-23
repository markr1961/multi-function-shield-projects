#define LED_OFF HIGH
#define LED_ON  LOW
#define BUTTON_DOWN true
#define BUTTON_RELEASED false

#define BUTTON1 A1
#define BUTTON2 A2
#define BUTTON3 A3

#define POT1    A0
#define pinA    A1  // Connected to CLK on KY-040
#define pinB    A2  // Connected to DT on KY-040
#define pinSW   A3  // Connected to SW on KY-040

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

int encoderPosCount = 90;
int LastPos = 90;
int pinALast;
int aVal;
boolean bCW;

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
  analogWrite(LED4, encoderPosCount);
  Serial.begin(115200);

  myservo.attach(9);  // attaches the servo on pin 9 to the servo object

  InitDisplay();

  pinMode(pinA,INPUT);
  pinMode(pinB,INPUT);
  /* Read Pin A
  Whatever state it's in will reflect the last position
  */
  pinALast = digitalRead(pinA);

}

void loop()
{
  static int lastPos = -1;

  aVal = digitalRead(pinA);
  if (aVal != pinALast)  // Means the knob is rotating
  {
    // if the knob is rotating, we need to determine direction
    // We do that by reading pin B.
    if (digitalRead(pinB) != aVal)    // Means pin A Changed first - We're Rotating Clockwise
    {
      encoderPosCount ++;
      bCW = true;
    }
    else    // Otherwise B changed first and we're moving CCW
    {
      bCW = false;
      encoderPosCount--;
    }
    Serial.print ("R!");
    if (bCW)
      Serial.println ("CW");
    else
      Serial.println("CCW");

    if (encoderPosCount > UPPER_LIMIT)
      encoderPosCount = UPPER_LIMIT;
    else if (encoderPosCount < LOWER_LIMIT)
      encoderPosCount = LOWER_LIMIT;

    Serial.println(encoderPosCount);

  }
  pinALast = aVal;
  if (!digitalRead(BUTTON1))
    digitalWrite(LED1, LED_ON);
  else
    digitalWrite(LED1, LED_OFF);

  if (!digitalRead(BUTTON2))
    digitalWrite(LED2, LED_ON);
  else
    digitalWrite(LED2, LED_OFF);

  if (!digitalRead(BUTTON3))
  {
    digitalWrite(LED3, LED_ON);
    encoderPosCount = MIDDLE;
    if ( encoderPosCount != LastPos)
    {
      Serial.println(encoderPosCount);
      LastPos = MIDDLE;
    }

  }
  else
  {
    digitalWrite(LED3, LED_OFF);
  }

  displayValue(encoderPosCount);
  myservo.write(encoderPosCount);              // tell servo to go to position in variable 'pos'
  analogWrite(LED4, UPPER_LIMIT - encoderPosCount);

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

