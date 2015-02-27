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
const int hc595_ser = 2;
const int hc595_rclk = 3;
const int hc595_srclk = 4;
//const int hc595_oe = 6;
const int hc595_srclr = 5;

const int multiplex[] = {6, 7, 8, 9, 10, 11, 12, 13};

char displayText[9] = "";

// set the display type
const int displayType = display_SevenSegment;

// ****************************************
//            Seven Segment Code
// ****************************************

const int patterns[] = {
  B00111111, B00000110, B01011011, 
  B01001111, B01100110, B01101101,
  B01111101, B00000111, B01111111,
  B01101111};

void showDisplay7Seg()
{
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
//                Common Code
// ****************************************


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

  for (int x = 0; x < 8; x++) {
    pinMode (multiplex[x], OUTPUT);
    digitalWrite (multiplex[x], HIGH);
  }
}


void loop()
{
  displayText[0] = (hour() / 10) + 48;
  displayText[1] = (hour() % 10) + 48;  
  displayText[2] = (minute() / 10) + 48;
  displayText[3] = (minute() % 10) + 48;  
  displayText[4] = (second() / 10) + 48;
  displayText[5] = (second() % 10) + 48;  

//  Serial.println("Current time : " + String(hour()) + ":" + String(minute()) + ":" + String(second()) + " " + String(day()) + "/" + String(month()) + "-" + String(year()));
//  Serial.println("displayText : " + String(displayText));


  switch (displayType) {
  case display_SevenSegment:
    showDisplay7Seg();
    break;
  default:
    break;
  }
}



