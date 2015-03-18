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

char versionHeader[] = "!Kaddu Klock v0.1-alpha";

// Use the Time Library (http://www.pjrc.com/teensy/td_libs_Time.html)
#include <Time.h>

// define display types
const int display_SevenSegment = 1;
const int display_LEDMatrix = 2;
const int display_LCD = 4;

// define IO pins
const int hc595_ser = 4;
const int hc595_rclk = 5;
const int hc595_srclk = 6;
//const int hc595_oe = 6;
const int hc595_srclr = 7;

const int multiplex[] = {6, 7, 8, 9, 10, 11, 12, 14};

const int buttonMinus = 10;
const int buttonMode = 9;
const int buttonPlus = 8;

char displayText[9] = "";

int statusButtonMinus;
int statusButtonMode;
int statusButtonPlus;

int modeStatus = 0;
int modeButtonPress = 0;

// Define display mode
const byte displayMode_Time = 1;
const byte displayMode_Date = 2;
const byte displayMode_Year = 3;
const byte displayMode_Overrun = 4;     // Pseudo displayMode. Should always be one bigger than the last, normal displayMode

const byte displayMode_SetHour = 129;
const byte displayMode_SetMinute = 130;
const byte displayMode_SetDay = 131;
const byte displayMode_SetMonth = 132;
const byte displayMode_SetYear = 133;
const byte displayMode_SetOverrun = 134;     // Pseudo displayMode_Set. Should always be one bigger than the last, normal displayMode_Set


// set the display mode
int displayMode = displayMode_Time;

// set the display type
//const int displayType = display_SevenSegment;
const int displayType = display_LEDMatrix;

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


// ****************************************
//                LED Matrix Code
// ****************************************

void showDisplayLEDMatrix()
{
  byte const patterns [64][8] = {
    00, 00, 00, 00, 00, 00, 00, 00,     // " " (space)
    00, 00, 00, 00, 00, 00, 00, 00,     // " " 
    00, 00, 00, 00, 00, 00, 00, 00,     // " " 
    00, 00, 00, 00, 00, 00, 00, 00,     // " " 
    00, 00, 00, 00, 00, 00, 00, 00,     // " " 
    00, 00, 00, 00, 00, 00, 00, 00,     // " " 
    00, 00, 00, 00, 00, 00, 00, 00,     // " " 
    00, 00, 00, 00, 00, 00, 00, 00,     // " " 
    00, 00, 00, 00, 00, 00, 00, 00,     // " " 
    00, 00, 00, 00, 00, 00, 00, 00,     // " " 
    00, 00, 00, 00, 00, 00, 00, 00,     // " " 
    00, 00, 00, 00, 00, 00, 00, 00,     // " " 
    00, 00, 00, 00, 00, 00, 00, 00,     // " " 
    00, 00, 00, 00, 00, 00, 00, 00,     // " " 
    00, 00, 00, 00, 00, 00, 00, 00,     // " " 
    00, 00, 00, 00, 00, 00, 00, 00,     // " " 
    04, 10, 10, 10, 10, 10, 04, 00,     // "0"
    04, 12, 04, 04, 04, 04, 14, 00,     // "1"
    04, 10, 02, 02, 04,  8, 14, 00,     // "2"
    04, 10, 02, 04, 02, 10, 04, 00,     // "3"
    10, 10, 10, 14, 02, 02, 02, 00,     // "4"
    14,  8,  12, 02, 02, 10, 04, 00,     // "5"
    04, 10,  8, 12, 10, 10, 04, 00,     // "6"
    14, 10, 02, 02, 02, 02, 02, 00,     // "7"
    04, 10, 10, 04, 10, 10, 04, 00,     // "8"
    04, 10, 10, 06, 02, 10, 04, 00,     // "9"
    00, 00, 00, 00, 00, 00, 00, 00,     // " " 
    00, 00, 00, 00, 00, 00, 00, 00,     // " " 
    00, 00, 00, 00, 00, 00, 00, 00,     // " " 
    00, 00, 00, 00, 00, 00, 00, 00,     // " " 
    00, 00, 00, 00, 00, 00, 00, 00,     // " " 
    00, 00, 00, 00, 00, 00, 00, 00,     // " " 
    00, 00, 00, 00, 00, 00, 00, 00,     // "A" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "B" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "C" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "D" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "E" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "F" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "G" 
    238, 68, 68, 124, 68, 68, 238, 00,     // "H" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "I" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "J" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "K" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "L" 
    120, 198, 168, 146, 130, 130, 130, 00,     // "M" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "N" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "O" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "P" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "Q" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "R" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "S" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "T" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "U" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "V" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "W" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "X" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "y" 
    00, 00, 00, 00, 00, 00, 00, 00,     // "Z" 
    00, 00, 00, 00, 00, 00, 00, 00};
  
  
  byte pattern;
  

  for (byte row = 0; row < 8; row++) {

    digitalWrite (hc595_rclk, LOW);
    digitalWrite (hc595_rclk, HIGH);

    digitalWrite (hc595_srclr, LOW);       // clear the 74hc595 serial register
    digitalWrite (hc595_srclr, HIGH);

    digitalWrite (hc595_rclk, LOW);

    pattern = patterns[displayText[0]-32][row];
    pattern = pattern << 4;
    pattern = pattern + patterns[displayText[1]-32][row];
    if (displayText[5] & 1) pattern++;
    pattern = 255 - pattern;
    shiftOut (hc595_ser, hc595_srclk, MSBFIRST, pattern);

    pattern = patterns[displayText[2]-32][row];
    pattern = pattern << 4;
    pattern = pattern + patterns[displayText[3]-32][row];
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


void setup()
{
//  Serial.begin(9600);                                                 // open serial connection
//  Serial.println(versionHeader);

  setTime(12, 0, 0, 1, 1, 2015);

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

  pinMode (buttonMinus, INPUT);
  pinMode (buttonMode, INPUT);
  pinMode (buttonPlus, INPUT);

  pinMode (13, OUTPUT);       // Use internal LED on pin 13 as debug display

  digitalWrite (buttonMinus, HIGH);        // Enable internal pull-up resistor
  digitalWrite (buttonMode, HIGH);        // Enable internal pull-up resistor
  digitalWrite (buttonPlus, HIGH);        // Enable internal pull-up resistor
}


void loop()
{
  switch (displayMode) {
    case displayMode_Time:
      displayText[0] = (hour() / 10) + 48;
      displayText[1] = (hour() % 10) + 48;  
      displayText[2] = (minute() / 10) + 48;
      displayText[3] = (minute() % 10) + 48;  
      displayText[4] = (second() / 10) + 48;
      displayText[5] = (second() % 10) + 48;
      break;
    case displayMode_Date:
      displayText[0] = (day() / 10) + 48;
      displayText[1] = (day() % 10) + 48;
      displayText[2] = (month() / 10) + 48;
      displayText[3] = (month() % 10) + 48;
      displayText[4] = 32;
      displayText[5] = 32;
      break;
    case displayMode_Year:
      displayText[0] = (year() / 1000) + 48;
      displayText[1] = ((year() % 1000) / 100) + 48;
      displayText[2] = ((year() % 100) / 10) + 48;
      displayText[3] = (year() % 10) + 48;
      displayText[4] = 32;
      displayText[5] = 32;
      break;
    case displayMode_SetHour:
      displayText[0] = (hour() / 10) + 48;
      displayText[1] = (hour() % 10) + 48;  
      displayText[2] = 'H';
      displayText[3] = 'R';  
      displayText[4] = 32;
      displayText[5] = 32;
      break;
    case displayMode_SetMinute:
      displayText[0] = 'M';
      displayText[1] = 'I';  
      displayText[2] = (minute() / 10) + 48;
      displayText[3] = (minute() % 10) + 48;  
      displayText[4] = 32;
      displayText[5] = 32;
      break;
    case displayMode_SetDay:
      displayText[0] = (day() / 10) + 48;
      displayText[1] = (day() % 10) + 48;  
      displayText[2] = 'D';
      displayText[3] = 'A';  
      displayText[4] = 32;
      displayText[5] = 32;
      break;
    case displayMode_SetMonth:
      displayText[0] = 'M';
      displayText[1] = 'O';  
      displayText[2] = (month() / 10) + 48;
      displayText[3] = (month() % 10) + 48;  
      displayText[4] = 32;
      displayText[5] = 32;
      break;
    case displayMode_SetYear:
      displayText[0] = (year() / 1000) + 48;
      displayText[1] = ((year() % 1000) / 100) + 48;
      displayText[2] = ((year() % 100) / 10) + 48;
      displayText[3] = (year() % 10) + 48;
      displayText[4] = 32;
      displayText[5] = 32;
      break;
    default:     // default should not happen, just here for precaution.
      break;
  }

//  Serial.println("Current time : " + String(hour()) + ":" + String(minute()) + ":" + String(second()) + " " + String(day()) + "/" + String(month()) + "-" + String(year()));
//  Serial.println("displayText : " + String(displayText));

  switch (displayType) {
  case display_SevenSegment:
    showDisplay7Seg();
    break;
  case display_LEDMatrix:
    showDisplayLEDMatrix();
    break;
  default:
    break;
  }

  statusButtonMinus = digitalRead (buttonMinus);
  statusButtonMode = digitalRead (buttonMode);
  statusButtonPlus = digitalRead (buttonPlus);

  if (statusButtonMode == LOW)
  {
    digitalWrite(13, HIGH);
    modeButtonPress++;
    if (modeButtonPress >= 200)
    {
      displayMode = displayMode_SetHour;
      
      modeButtonPress = 0;
    }
  }
  else
  {
    digitalWrite(13, LOW);
    if (modeButtonPress >= 10)
     {
       displayMode++;
       modeButtonPress = 0;
     } 
     else
     {
       if ((displayMode > 1) && (displayMode < 128))     // if we display something else than time
       {
         modeStatus++;
         
         if (modeStatus > 300)     // wait a few seconds before changing the displayMode
         {
           displayMode++;
           modeStatus = 0;
         }
       } else if (displayMode > 128)
       {
         modeStatus++;
         
         if (modeStatus > 1000)     // wait a few seconds before changing the displayMode
         {
           displayMode = displayMode_Time;
           modeStatus = 0;
         }
       }
     }
    if (displayMode == displayMode_Overrun)     // if we have moved beyond the last displayMode
    {
      displayMode = displayMode_Time;
    } else if (displayMode == displayMode_SetOverrun)     // if we have moved beyond the last displayMode
    {
      displayMode = displayMode_SetHour;
    }

  }
}

