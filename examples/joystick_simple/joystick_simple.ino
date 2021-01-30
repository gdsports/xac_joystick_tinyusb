/************************************************************************
MIT License

Copyright (c) 2021 gdsports625@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*************************************************************************/
#include <Bounce2.h>
#include "xac_joystick_tinyusb.h"

// This will not work on a Trinket M0 because it does not have enough
// pins.
#define NUM_BUTTONS 8
const uint8_t BUTTON_PINS[NUM_BUTTONS] = {2, 3, 4, 5, 6, 7, 8, 9};

Bounce * buttons = new Bounce[NUM_BUTTONS];

TUJoystick Joystick;

void setup()
{
  Joystick.begin();

  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].attach( BUTTON_PINS[i], INPUT_PULLUP );
  }

  // wait until device mounted
  while( !USBDevice.mounted() ) delay(1);
}


void loop()
{
  if ( !Joystick.ready() ) return;

  for (int i = 0; i < NUM_BUTTONS; i++) {
    // Update the Bounce instance :
    buttons[i].update();
    // Button fell means button pressed
    if ( buttons[i].fell() ) {
      Joystick.press(i);
    }
    else if ( buttons[i].rose() ) {
      Joystick.release(i);
    }
  }

  // A0 = x axis, 0=left, 255=right
  // A1 = y axis, 0=forward, 255=back
  // This depends on your joystick.
  Joystick.xAxis((uint8_t)map(analogRead(A0), 0, 1023, 0, 255));
  Joystick.yAxis((uint8_t)map(analogRead(A1), 0, 1023, 255, 0));

  // Functions above only set the values.
  // This writes the report to the host.
  Joystick.loop();
}
