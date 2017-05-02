#ifndef buttons
#define buttons

#include "Arduino.h"

class buttons
{
  public:
    buttons ();
    void update ();
    bool up();
    bool down();
    bool left();
    bool right();
    bool a();
    bool b();
    bool longPressA();
  private:
    const int buttonUp = 2;       // datapin no the button is connected to
    const int buttonDown = 3;     // datapin no the button is connected to
    const int buttonLeft = 4;     // datapin no the button is connected to
    const int buttonRight = 5;    // datapin no the button is connected to
    const int buttonA = 6;        // datapin no the button is connected to
    const int buttonB = 7;        // datapin no the button is connected to
    unsigned long timeSincePressed[6] = {0, 0, 0, 0, 0, 0};     // milliseconds since the key was pressed. 0 if the key is depressed
  
    byte status = 0;
}
#endif
