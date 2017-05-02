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

//#define debug_serial

// define display types - uncomment the line which reflects the connected display
//#define display_SevenSegment
//#define display_LEDMatrix
//#define display_LCD
#define display_OLED

// Use the Time Library (http://www.pjrc.com/teensy/td_libs_Time.html)
#include <Time.h>
#include <TimeLib.h>

#ifdef display_OLED
// Use the u8g2 lib for display on OLED
#include <U8g2lib.h>
#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
#endif

const char versionHeader[] = "!Kaddu Klock v0.1-alpha oled";

#ifdef display_LEDMatrix
// define IO pins for LEDMatrix
const int hc595_ser = 4;
const int hc595_rclk = 5;
const int hc595_srclk = 6;
const int hc595_srclr = 7;

const int multiplex[] = {6, 7, 8, 9, 10, 11, 12, 13};
#endif

#include "buttons.h"
buttons pushButtons = buttons();

const int displayModeTime = 1;
const int displayModeDate = 2;
const int displayModeTimeSet = 3;
const int displayModeDateSet = 4;
int displayMode = displayModeTime;

char displayText[] = "1234567890"; // Placeholder for displayed text

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
  Serial.begin(9600);                   // open serial connection
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
  pinMode (hc595_srclr, OUTPUT);

  for (int x = 0; x < 7; x++) {
    pinMode (multiplex[x], OUTPUT);
    digitalWrite (multiplex[x], HIGH);
  }
#endif
  
  DDRD = DDRD & B00000011; // Set pins 7-2 to INPUT - leave pins 1 & 0 unchanged (serial pins)
  PORTD = (PORTD & B00000011) | B11111100; // Set pins 7-2 HIGH (turn on internal pull-up resistor)
}

void settingTime()
{
  if (pushButtons.up())  // buttonUp
  {
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

  if (pushButtons.down())  // buttonDown
  {
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

  if (pushButtons.left())  // buttonLeft
  {
    pos--;
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

  if (pushButtons.right())  // buttonRight
  {
    pos++;
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

  if (pushButtons.a())  // buttonA
  {
  }

  if (pushButtons.b())  // buttonB
  {
  }
}

void formTime (char* txt)
{
  char tmpTxt[11] = "00:00:00";

  tmpTxt[0] = (hour() / 10) + 48;
  tmpTxt[1] = (hour() % 10) + 48;  
  tmpTxt[3] = (minute() / 10) + 48;
  tmpTxt[4] = (minute() % 10) + 48;  
  tmpTxt[6] = (second() / 10) + 48;
  tmpTxt[7] = (second() % 10) + 48;  
  strcpy (txt, tmpTxt);
}

void formDate (char* txt)
{
  char tmpTxt[11] = "00/00-2000";

  tmpTxt[0] = (day() / 10) + 48;
  tmpTxt[1] = (day() % 10) + 48;  
  tmpTxt[3] = (month() / 10) + 48;
  tmpTxt[4] = (month() % 10) + 48;  
  tmpTxt[6] = (year() / 1000) + 48;
  tmpTxt[7] = ((year() / 100) % 10) + 48;  
  tmpTxt[8] = ((year() / 10) % 10) + 48;  
  tmpTxt[9] = (year() % 10) + 48;  
  strcpy (txt, tmpTxt);
}

void loop()
{
  pushButtons.update();
#ifdef debug_serial
  Serial.println("Current time : " + String(hour()) + ":" + String(minute()) + ":" + String(second()) + " " + String(day()) + "/" + String(month()) + "-" + String(year()));
  Serial.println("millis() : " + String (millis()));
#endif

  switch (displayMode) {
    case displayModeTime:
      if (pushButtons.longPressA()) {
        displayMode = displayModeTimeSet;
      }
      break;
    case displayModeTimeSet:
      settingTime();
      break;
  }

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
    switch (displayMode) {
      case displayModeTime: {
          u8g2.drawRFrame(0, 0, 127, 63, 3);
          u8g2.setFont(u8g2_font_inr16_mf);
          formTime(displayText);
          u8g2.drawStr(12, 30, displayText);
          formDate(displayText);
          u8g2.setFont(u8g2_font_7x14B_mf);
          u8g2.drawStr(29, 10, displayText);
        }
        break;
      case displayModeTimeSet: {
          u8g2.drawFrame(0, 0, 127, 63);
          u8g2.setFont(u8g2_font_inr16_mf);
          formTime(displayText);
          u8g2.drawStr(12, 20, displayText);
        }
        break;
    }
  } while ( u8g2.nextPage() );
#endif
}

