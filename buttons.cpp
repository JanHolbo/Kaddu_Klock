#include "Arduino.h"
#include "buttons.h"

// define the button digital pins
  const int buttonUp = 2;       // datapin no the button is connected to
  const int buttonDown = 3;     // datapin no the button is connected to
  const int buttonLeft = 4;     // datapin no the button is connected to
  const int buttonRight = 5;    // datapin no the button is connected to
  const int buttonA = 6;        // datapin no the button is connected to
  const int buttonB = 7;        // datapin no the button is connected to
  unsigned long timeSincePressed[6] = {0, 0, 0, 0, 0, 0};     // milliseconds since the key was pressed. 0 if the key is depressed
  
  byte status = 0;
  
  buttons::buttons() {
    status = (~PIND) & B11111100;  // read all 6 input pins buttons are connected at once through the hardware register
  }

  void buttons::update() {
    byte oldStatus=status;

    status = (~PIND) & B11111100;  // read all 6 input pins buttons are connected at once through the hardware register

#ifdef debug_serial
  Serial.println("buttons : " + String (status));
#endif
    // do an oldStatus XOR status to see if anything has changed 
    if (oldStatus^status)
    {
#ifdef debug_serial
  Serial.println("button state changed - oldStatus : " + String (oldStatus) + " status : " + String (status) + " XOR : " + String(oldStatus^status));
#endif
      byte bitmask = 2;
      for (int x = 2; x < 8; x++)        // cycle through the bits of the button status byte. Should not be hardcoded like this :-/
      {
        bitmask = bitmask<<1;
#ifdef debug_serial
  Serial.println("button status : " + String (status) + " bitmask : " + String (bitmask) + " status & bitmask : " + String(status & bitmask));
  Serial.println("button oldStatus : " + String (oldStatus) + " bitmask : " + String (bitmask) + " !oldStatus & bitmask : " + String(!oldStatus & bitmask));
#endif
        if (status & bitmask) {
          if ((oldStatus ^ status) & bitmask) {
            timeSincePressed[x-2] = millis();
          }
        }

        if (!oldStatus & bitmask) {
          if ((oldStatus ^ status) & bitmask) {
            timeSincePressed[x-2] = 0;
          }
        }
        
#ifdef debug_serial
  Serial.println("button timeSincePressed[" + String (x-2) + "] contains " + String (timeSincePressed[x-2]));
#endif

      }
    }
  }

  bool buttons::up() {
    if (status && 1>>buttonUp) return (true);
    return (false);
  }

  bool buttons::down() {
    if (status && 1>>buttonDown) return (true);
    return (false);
  }

  bool buttons::left() {
    if (status && 1>>buttonLeft) return (true);
    return (false);
  }

  bool buttons::right() {
    if (status && 1>>buttonRight) return (true);
    return (false);
  }

  bool buttons::a() {
    if (status && 1>>buttonA) return (true);
    return (false);
  }

  bool buttons::b() {
    if (status && 1>>buttonB) return (true);
    return (false);
  }

  bool buttons::longPressA() {
    if (timeSincePressed[buttonA-2] > 0) {
      if ((millis() - timeSincePressed[buttonA-2]) >= 500) {
        return (true);
      }
    }
        
    return (false);
  }
  
};

