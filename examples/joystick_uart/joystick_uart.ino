/*
 * MIT License
 *
 * Copyright (c) 2021 gdsports625@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/*
 * XAC Joystick
 *
 * Emulate a USB joystick with 8 buttons and 2 axes. Designed to work with
 * the Xbox Adaptive Controller.
 * Receive joystick event input on Serial1 from another computer. Works
 * on Adafruit Trinket M0 and other SAMD21 boards.
 *
 * The format in the serial port looks like this.
 * <STX> <length> <type> 8 bytes of binary data <ETX>
 * 0x02    0x09    0x02                          0x03
 *
 * The type least significant 4 bits should be 2 for joysticks. The 4 most
 * significant bits specify an index in case multiple gadgets are on the
 * the same bus. For example, gadget 0 ignores packets with the index !=0.
 * gadget 1 ignores packets with the index != 1.
 *
 * The 8 bytes of binary data is the joystick USB HID report.
 *
 * struct JoystickEventData
 * {
 *   uint8_t  buttons;
 *   uint8_t  xAxis;
 *   uint8_t  yAxis;
 *   uint8_t  filler[5];
 * }__attribute__((packed));
 *
 * The Joystick library is based on the tinyusb stack as used in
 * Adafruit SAMD and nRF52 board support packages.
 */

#define HAS_DOTSTAR_LED (defined(ADAFRUIT_TRINKET_M0) || defined(ADAFRUIT_ITSYBITSY_M0) || defined(ADAFRUIT_ITSYBITSY_M4_EXPRESS))
#if HAS_DOTSTAR_LED
#include <Adafruit_DotStar.h>
#if defined(ADAFRUIT_ITSYBITSY_M4_EXPRESS)
#define DATAPIN    8
#define CLOCKPIN   6
#elif defined(ADAFRUIT_ITSYBITSY_M0)
#define DATAPIN    41
#define CLOCKPIN   40
#elif defined(ADAFRUIT_TRINKET_M0)
#define DATAPIN    7
#define CLOCKPIN   8
#endif
Adafruit_DotStar strip = Adafruit_DotStar(1, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
#endif

#include "xac_joystick_tinyusb.h"

TUJoystick Joystick;

// On SAMD boards where the native USB port is also the serial console, use
// Serial1 for the serial console. This applies to all SAMD boards except for
// Arduino Zero and M0 boards.
#if defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAM_ZERO)
#define SerialDebug SERIAL_PORT_MONITOR
#else
#define SerialDebug Serial1
#endif

#define JS_UART    Serial1
#define jsbegin(...)      JS_UART.begin(__VA_ARGS__)
#define jsprint(...)      JS_UART.print(__VA_ARGS__)
#define jswrite(...)      JS_UART.write(__VA_ARGS__)
#define jsprintln(...)    JS_UART.println(__VA_ARGS__)
#define jsread(...)       JS_UART.read(__VA_ARGS__)
#define jsreadBytes(...)  JS_UART.readBytes(__VA_ARGS__)
#define jsavailable(...)  JS_UART.available(__VA_ARGS__)
#define jssetTimeout(...) JS_UART.setTimeout(__VA_ARGS__)

#define DEBUG_ON  0
#if DEBUG_ON
#define dbbegin(...)      SerialDebug.begin(__VA_ARGS__)
#define dbprint(...)      SerialDebug.print(__VA_ARGS__)
#define dbprintln(...)    SerialDebug.println(__VA_ARGS__)
#define dbwrite(...)      SerialDebug.write(__VA_ARGS__)
#else
#define dbbegin(...)
#define dbprint(...)
#define dbprintln(...)
#define dbwrite(...)
#endif

uint32_t elapsed_mSecs(uint32_t last_millis)
{
  uint32_t now = millis();
  if (now < last_millis) {
    return (now + 1) + (0xFFFFFFFF - last_millis);
  }
  else {
    return now - last_millis;
  }
}

const uint8_t STX = 0x02;
const uint8_t ETX = 0x03;

uint8_t js_report(uint8_t *buffer, size_t buflen)
{
  static uint8_t js_buffer[32];
  static uint8_t js_buflen;
  static uint8_t js_state=0;
  static uint8_t js_expectedlen;
  static uint32_t timeout_ms;
  size_t bytesIn;

  int byt;

  while (jsavailable() > 0) {
    switch (js_state) {
      case 0:
        dbprint(js_state); dbprint(',');
        byt = jsread();
        if (byt != -1) {
          dbprintln(byt, HEX);
          if (byt == STX) {
            timeout_ms = millis();
            js_state = 1;
            js_buflen = 0;
          }
        }
        break;
      case 1:
        dbprint(js_state); dbprint(',');
        byt = jsread();
        if (byt != -1) {
          dbprintln(byt, HEX);
          js_buffer[0] = byt;
          js_buflen = 1;
          js_expectedlen = byt;
          if (js_expectedlen > (sizeof(js_buffer) - 1)) {
            js_expectedlen = sizeof(js_buffer) - 1;
          }
          js_state = 2;
        }
        break;
      case 2:
        dbprint(js_state); dbprint(',');
        byt = jsread();
        if (byt != -1) {
          dbprintln(byt, HEX);
          js_buffer[1] = byt;
          js_buflen = 2;
          js_state = 3;
        }
        break;
      case 3:
        dbprint(js_state); dbprint(',');
        bytesIn = jsreadBytes(&js_buffer[js_buflen], js_expectedlen - js_buflen + 1);
        dbprintln(bytesIn);
        if (bytesIn > 0) {
          js_buflen += bytesIn;
          if (js_buflen > js_expectedlen) {
            js_state = 4;
          }
        }
        break;
      case 4:
        dbprint(js_state); dbprint(',');
        byt = jsread();
        if (byt != -1) {
          dbprintln(byt, HEX);
          if (byt == ETX) {
            if (js_buflen > buflen) js_buflen = buflen;
            memcpy(buffer, js_buffer, js_buflen);
            js_state = 0;
            return js_buflen;
          }
          else if (byt == STX) {
            timeout_ms = millis();
            js_state = 1;
            js_buflen = 0;
            return 0;
          }
          js_state = 0;
        }
        break;
      default:
        dbprintln("jsread: invalid state");
        break;
    }
  }
  // If STX seen and more than 2 ms, give up and go back to looking for STX
  if ((js_state != 0) && (elapsed_mSecs(timeout_ms) > 2)) {
    js_state = 0;
    static uint32_t timeout_error = 0;
    timeout_error++;
    digitalWrite(LED_BUILTIN, timeout_error & 1);
  }
  return 0;
}

void setup()
{
  Joystick.begin();
  // Turn off built-in LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  jsbegin( 115200 );
  jssetTimeout(0);
  dbbegin( 115200 );
  dbprintln("Joystick_UART setup");

#if HAS_DOTSTAR_LED
  // Turn off built-in Dotstar RGB LED
  strip.begin();
  strip.clear();
  strip.show();
#endif

  // wait until device mounted
  while( !USBDevice.mounted() ) delay(1);
}

void loop()
{
  if ( !Joystick.ready() ) return;

  uint8_t jsData[32];
  uint8_t reportLen = js_report(jsData, sizeof(jsData));
  if ((reportLen > 1) && ((jsData[1] & 0x0F) == 2)) {
    Joystick.write(&jsData[2]);
  }
  else {
    Joystick.loop();
  }
}
