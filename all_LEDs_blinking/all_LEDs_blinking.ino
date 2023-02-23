#define LED_OFF HIGH
#define LED_ON  LOW

/* Define shift register pins used for seven segment display */
#define LATCH_DIO 4
#define CLK_DIO   7
#define DATA_DIO  8

#define POT1    A0
#define BUTTON1 A1
#define BUTTON2 A2
#define BUTTON3 A3

#define LED1 13
#define LED2 12
#define LED3 11
#define LED4 10

int LastPwmValue;
int blah = 0;

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
  LastPwmValue = 255 - (analogRead(POT1) / 4);
  analogWrite(LED4, LastPwmValue);
  Serial.begin(9600);

  /* Set DIO pins to outputs */
  pinMode(LATCH_DIO, OUTPUT);
  pinMode(CLK_DIO, OUTPUT);
  pinMode(DATA_DIO, OUTPUT);

}

void loop()
{
  int PotValue, PwmValue;

  PotValue = analogRead(POT1);

  /* Update the display with the current counter value */
  switch (blah)
  {
    case 0: // MSB
      WriteNumberToSegment(0 , PotValue / 1000);
      blah++;
      break;

    case 1:
      WriteNumberToSegment(1 , (PotValue / 100) % 10);
      blah++;
      break;

    case 2:
      WriteNumberToSegment(2 , (PotValue / 10) % 10);
      blah++;
      break;

    default:
    case 3: // LSB
      WriteNumberToSegment(3 , PotValue % 10);
      blah = 0;
      break;
  }

  if (!digitalRead(BUTTON1))
    digitalWrite(LED1, LED_ON);
  else
    digitalWrite(LED1, LED_OFF);

  if (!digitalRead(BUTTON2))
    digitalWrite(LED2, LED_ON);
  else
    digitalWrite(LED2, LED_OFF);

  if (!digitalRead(BUTTON3))
    digitalWrite(LED3, LED_ON);
  else
    digitalWrite(LED3, LED_OFF);

  PwmValue = 255 - (PotValue / 4);
  if (PwmValue != LastPwmValue)
  {
    //    Serial.println();
    analogWrite(LED4, PwmValue);
    //    if (PwmValue  > LastPwmValue )
    //      Serial.print("+");
    //    else
    //      Serial.print("-");
    LastPwmValue = PwmValue;
  }
  //  else
  //    Serial.print(".");

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

