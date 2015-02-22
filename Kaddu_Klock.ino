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

void setup()
{
  Serial.begin(9600);
  Serial.println(versionHeader);

  setTime(12, 0, 0, 1, 1, 2000);
}


void loop()
{
  Serial.println("Current time : " + hour() + ":" + minute() + ":" + second() + " " + day() + "/" + month() + "-" + year());

  delay(1000);
}


