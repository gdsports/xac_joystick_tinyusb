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
#include "xac_joystick_tinyusb.h"

TUJoystick Joystick;

void setup()
{
  Joystick.begin();

  // wait until device mounted
  while( !USBDevice.mounted() ) delay(1);
}


void loop()
{
  if ( !Joystick.ready() ) return;

  static uint8_t count = 0;
  // 8 buttons
  if (count > 7) {
    Joystick.releaseAll();
    count = 0;
  }
  Joystick.press(count);
  count++;

  // Move x/y Axis to a random position
  Joystick.xAxis(random(256));
  Joystick.yAxis(random(256));

  // Functions above only set the values.
  // This writes the report to the host.
  Joystick.loop();
  delay(100);
}
