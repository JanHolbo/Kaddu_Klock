/* 
 * Kaddu Klock
 * http://www.kaddu.dk/index.php/en/projects/Kaddu_Klock
 * GitHub Repository: https://github.com/JanHolbo/Kaddu_Klock
 * (C) 2014 by Jan Rasmussen & Kaddu.dk jan at kaddu dot dk
 * This file and others in this pack is - where not otherwise stated - 
 * covered by the GPL v2 license.
 * Please feel free to fork this project and please submit patches and 
 * feature requests to the above email 
 *
 * File: Kaddu_Klock/Kaddu_Klock.ino
 * Version: 0.1-alpha
 *
 * This is the main Kaddu Klock file
 *
 */

// Use the Time Library (http://www.pjrc.com/teensy/td_libs_Time.html)
#include <Time.h>
#include <TimeLib.h>

// Use the u8g2 lib for display on OLED
#include <U8g2lib.h>
#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
  
const char versionHeader[] = "!Kaddu Klock v0.1-alpha oled";

//#define debug_serial

// define display types - uncomment the line which reflects the connected display
//#define display_SevenSegment
//#define display_LEDMatrix
//#define display_LCD
#define display_OLED

#ifdef display_LEDMatrix
// define IO pins for LEDMatrix
const int hc595_ser = 4;
const int hc595_rclk = 5;
const int hc595_srclk = 6;
//const int hc595_oe = 6;
const int hc595_srclr = 7;

const int multiplex[] = {6, 7, 8, 9, 10, 11, 12, 13};
#endif

// define the button digital pins
const int buttonUp = 2;
const int buttonDown = 3;
const int buttonLeft = 4;
const int buttonRight = 5;
const int buttonA = 6;
const int buttonB = 7;

char displayText[9] = "00:00:00"; // Placeholder for displayed text

int pos = 1; // position marker (used when setting time or date)

#ifdef display_SevenSegment
// ****************************************
//            Seven Segment Code
// ****************************************
void showDisplay7Seg()
{
const byte patterns[] = {
  B00111111, B00000110, B01011011, 
  B01001111, B01100110, B01101101,
  B01111101, B00000111, B01111111,
  B01101111};

  for (int x = 0; x < 6; x++) {
    digitalWrite (multiplex[x], HIGH);
  }

  for (int digit = 0; digit < 6; digit++) {

    int pattern = patterns[displayText[digit]-48];

    digitalWrite (hc595_rclk, LOW);
    digitalWrite (hc595_rclk, HIGH);

    digitalWrite (hc595_srclr, LOW);       // clear the 74hc595 serial register
    digitalWrite (hc595_srclr, HIGH);

    digitalWrite (hc595_rclk, LOW);
    shiftOut (hc595_ser, hc595_srclk, MSBFIRST, pattern);
    digitalWrite (hc595_rclk, HIGH);

    digitalWrite (multiplex[digit], LOW);   // turn on the right digit

    delayMicroseconds (1000);

    digitalWrite (multiplex[digit], HIGH);   // turn on the right digit

    digitalWrite (hc595_rclk, LOW);
    digitalWrite (hc595_rclk, HIGH);
  }
}
#endif

#ifdef display_LEDMatrix
// ****************************************
//                LED Matrix Code
// ****************************************
void showDisplayLEDMatrix()
{
  byte const patterns [10][8] = {04, 10, 10, 10, 10, 10, 04, 00,
                                 04, 12, 04, 04, 04, 04, 14, 00,
                                 04, 10, 02, 02, 04,  8, 14, 00,
                                 04, 10, 02, 04, 02, 10, 04, 00,
                                 10, 10, 10, 14, 02, 02, 02, 00,
                                 14,  8,  8, 04, 02, 10, 04, 00,
                                 04, 10,  8, 12, 10, 10, 04, 00,
                                 14, 10, 02, 02, 02, 02, 02, 00,
                                 04, 10, 10, 04, 10, 10, 04, 00,
                                 04, 10, 10, 06, 02, 10, 04, 00};
  
  byte pattern;

  for (byte row = 0; row < 8; row++) {
    digitalWrite (hc595_rclk, LOW);
    digitalWrite (hc595_rclk, HIGH);

    digitalWrite (hc595_srclr, LOW);       // clear the 74hc595 serial register
    digitalWrite (hc595_srclr, HIGH);

    digitalWrite (hc595_rclk, LOW);

    pattern = patterns[displayText[0]-48][row];
    pattern = pattern << 4;
    pattern = pattern + patterns[displayText[1]-48][row];
    if (displayText[5] & 1) pattern++;
    pattern = 255 - pattern;
    shiftOut (hc595_ser, hc595_srclk, MSBFIRST, pattern);

    pattern = patterns[displayText[2]-48][row];
    pattern = pattern << 4;
    pattern = pattern + patterns[displayText[3]-48][row];
    pattern = 255 - pattern;
    shiftOut (hc595_ser, hc595_srclk, MSBFIRST, pattern);

    pattern = 1 << row; // row select
    shiftOut (hc595_ser, hc595_srclk, MSBFIRST, pattern);

    digitalWrite (hc595_rclk, HIGH);

    delayMicroseconds (1000);
//    delay (1000);

    digitalWrite (hc595_rclk, LOW);
    digitalWrite (hc595_rclk, HIGH);
  }
}
#endif
  
// ****************************************
//                Common Code
// ****************************************
void setup()
{
#ifdef debug_serial
  Serial.begin(9600);                                                 // open serial connection
  Serial.println(versionHeader);
#endif
  
  u8g2.begin();
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);

  setTime(12, 0, 0, 1, 1, 2015);

#ifdef display_LEDMatrix
// Setup digital pins for 74hc595
  pinMode (hc595_ser, OUTPUT);
  pinMode (hc595_rclk, OUTPUT);
  pinMode (hc595_srclk, OUTPUT);
  //  pinMode (hc595_oe, OUTPUT);
  pinMode (hc595_srclr, OUTPUT);

  for (int x = 0; x < 7; x++) {
    pinMode (multiplex[x], OUTPUT);
    digitalWrite (multiplex[x], HIGH);
  }
#endif
  
//  pinMode (buttonLeft, INPUT_PULLUP);   // Set internal pullup resistor
//  pinMode (buttonRight, INPUT_PULLUP);  // Set internal pullup resistor
//  pinMode (buttonUp, INPUT_PULLUP);     // Set internal pullup resistor
//  pinMode (buttonDown, INPUT_PULLUP);   // Set internal pullup resistor
//  pinMode (buttonA, INPUT_PULLUP);      // Set internal pullup resistor
//  pinMode (buttonB, INPUT_PULLUP);      // Set internal pullup resistor
  DDRD = DDRD & B00000011; // Set pins 7-2 to INPUT - leave pins 1 & 0 unchanged (serial pins)
  PORTD = (PORTD & B00000011) | B11111100; // Set pins 7-2 HIGH (turn on internal pull-up resistor)
}

void loop()
{
  byte buttons = 0;

  buttons = (~PIND) & B11111100;  // read all 6 input pins buttons are connected at once through the hardware register

//  if (!(digitalRead(buttonUp))) buttons+=4;
//  if (!(digitalRead(buttonDown))) buttons+=8;
//  if (!(digitalRead(buttonLeft))) buttons+=16;
//  if (!(digitalRead(buttonRight))) buttons+=32;
//  if (!(digitalRead(buttonA))) buttons+=64;
//  if (!(digitalRead(buttonB))) buttons+=128;

  displayText[0] = (hour() / 10) + 48;
  displayText[1] = (hour() % 10) + 48;  
  displayText[3] = (minute() / 10) + 48;
  displayText[4] = (minute() % 10) + 48;  
  displayText[6] = (second() / 10) + 48;
  displayText[7] = (second() % 10) + 48;  

#ifdef debug_serial
  Serial.println(buttons); // show buttons pressed
#endif

  if (buttons & (1>>buttonUp))  // buttonUp
  {
// subtract seconds depending on position
//    if (pos == 0) adjustTime(5*60*60);
//    if (pos == 0) adjustTime(5*60*60);
//    if (pos == 1) adjustTime(1*60*60);
//    if (pos == 3) adjustTime(10*60);
//    if (pos == 4) adjustTime(1*60);
//    if (pos == 6) adjustTime(10);
//    if (pos == 7) adjustTime(1);

    switch (pos) {
      case 1:
        adjustTime(1*60*60);
        break;
      case 3:
        adjustTime(10*60);
        break;
      case 4:
        adjustTime(1*60);
        break;
      case 6:
        adjustTime(10);
        break;
      case 7:
        adjustTime(1);
        break;
    }
  }

  if (buttons & (1>>buttonDown))  // buttonDown
  {
// subtract seconds depending on position
//    if (pos == 0) adjustTime(-5*60*60);
//    if (pos == 0) adjustTime(-5*60*60);
//    if (pos == 1) adjustTime(-1*60*60);
//    if (pos == 3) adjustTime(-10*60);
//    if (pos == 4) adjustTime(-1*60);
//    if (pos == 6) adjustTime(-10);
//    if (pos == 7) adjustTime(-1);
    switch (pos) {
      case 1:
        adjustTime(-1*60*60);
        break;
      case 3:
        adjustTime(-10*60);
        break;
      case 4:
        adjustTime(-1*60);
        break;
      case 6:
        adjustTime(-10);
        break;
      case 7:
        adjustTime(-1);
        break;
    }
  }

#ifdef debug_serial
  Serial.println("buttonLeft : " + String (buttons & (16)));
#endif

  if (buttons & (1>>buttonLeft))  // buttonLeft
  {
    pos--;
//    if (pos == 2) pos--;
//    if (pos == 5) pos--;
//    if (pos < 1) pos = 7;
    switch (pos) {
      case 0:
        pos = 7;
        break;
      case 2:
      case 5:
        pos--;
        break;
    }
  }

//  Serial.println("buttonRight : " + String (buttons & (32)));
  if (buttons & (1>>buttonRight))  // buttonRight
  {
    pos++;
//    if (pos == 2) pos++;
//    if (pos == 5) pos++;
//    if (pos > 7) pos = 1;
    switch (pos) {
      case 2:
      case 5:
        pos++;
        break;
      case 8:
        pos = 1;
        break;
    }
  }

  if (buttons & (1>>buttonA))  // buttonA
  {
  }

  if (buttons & (1>>buttonB))  // buttonB
  {
  }

#ifdef debug_serial
  Serial.println("Current time : " + String(hour()) + ":" + String(minute()) + ":" + String(second()) + " " + String(day()) + "/" + String(month()) + "-" + String(year()));
  Serial.println("displayText : " + String(displayText));
#endif

#ifdef display_SevenSegment
    showDisplay7Seg();
#endif

#ifdef display_LEDMatrix
    showDisplayLEDMatrix();
#endif

/* 
 * OLED Code 
 */
#ifdef display_OLED
  u8g2.firstPage();
  do {
    u8g2.drawFrame(0, 0, 127, 63);
    u8g2.setFont(u8g2_font_inb16_mf);
    u8g2.drawStr( 5, 20, displayText);
    u8g2.drawTriangle (5+(pos*14), 18, 16+(pos*14), 18, 11+(pos*14), 10);
    u8g2.drawTriangle (5+(pos*14), 40, 16+(pos*14), 40, 11+(pos*14), 48);
  } while ( u8g2.nextPage() );
#endif
//  delay (100);
}

