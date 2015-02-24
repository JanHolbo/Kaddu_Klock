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
const int hc595_rclk = 5;
const int hc595_srclk = 4;
const int hc595_oe = 6;
const int hc595_srclr = 7;

char displayText[9] = "";

// set the display type
const int displayType = display_SevenSegment;

// ****************************************
//            Seven Segment Code
// ****************************************

const int patterns[] = {B00111111, B00000110, B01011011, 
                       B01001111, B01100110, B01101101,
                       B01111101, B00000111, B01111111,
                       B01101111};

void showDisplay7Seg()
{
  int pattern = patterns[displayText[5]-48];

  digitalWrite (hc595_srclk, LOW);
  shiftOut (hc595_ser, hc595_rclk, MSBFIRST, pattern);
  digitalWrite (hc595_srclk, HIGH);
}


// ****************************************
//                Common Code
// ****************************************


void setup()
{
  Serial.begin(9600);                                                 // open serial connection
  Serial.println(versionHeader);

  setTime(12, 0, 0, 1, 1, 2015);
  
  // Setup digital pins for 74hc595
  pinMode (hc595_ser, OUTPUT);
  pinMode (hc595_rclk, OUTPUT);
  pinMode (hc595_srclk, OUTPUT);
  pinMode (hc595_oe, OUTPUT);
  pinMode (hc595_srclr, OUTPUT);
}


void loop()
{
  displayText[0] = (hour() / 10) + 48;
  displayText[1] = (hour() % 10) + 48;  
  displayText[2] = (minute() / 10) + 48;
  displayText[3] = (minute() % 10) + 48;  
  displayText[4] = (second() / 10) + 48;
  displayText[5] = (second() % 10) + 48;  

  Serial.println("Current time : " + String(hour()) + ":" + String(minute()) + ":" + String(second()) + " " + String(day()) + "/" + String(month()) + "-" + String(year()));
  Serial.println("displayText : " + String(displayText));


  switch (displayType) {
    case display_SevenSegment:
        showDisplay7Seg();
      break;
    default:
      break;
  }
  

  delay(1000);
}


