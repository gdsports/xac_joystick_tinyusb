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
#include "Adafruit_TinyUSB.h"

#pragma once

#define ATTRIBUTE_PACKED  __attribute__((packed, aligned(1)))

typedef struct ATTRIBUTE_PACKED {
    uint8_t buttons;
    uint8_t	xAxis;
    uint8_t	yAxis;
} HID_JoystickReport_Data_t;

// HID report descriptor 2 axes, 8 buttons
// Single Report (no ID) descriptor
uint8_t const desc_hid_report[] =
{
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x04,        // Usage (Joystick)
    0xA1, 0x01,        // Collection (Application)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x35, 0x00,        //   Physical Minimum (0)
    0x45, 0x01,        //   Physical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x08,        //   Report Count (8)
    0x05, 0x09,        //   Usage Page (Button)
    0x19, 0x01,        //   Usage Minimum (0x01)
    0x29, 0x08,        //   Usage Maximum (0x08)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x46, 0xFF, 0x00,  //   Physical Maximum (255)
    0x09, 0x30,        //   Usage (X)
    0x09, 0x31,        //   Usage (Y)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x02,        //   Report Count (2)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
};


class TUJoystick {
  public:
    inline TUJoystick(void);

    inline void begin(void);
    inline void end(void);
    inline void loop(void);
    inline void write(void);
    inline void write(void *report);
    inline void press(uint8_t b);
    inline void release(uint8_t b);
    inline void releaseAll(void);

    inline void buttons(uint8_t b);
    inline void xAxis(uint8_t a);
    inline void yAxis(uint8_t a);
    inline bool ready(void) { return this->usb_hid.ready(); };

    // Sending is public for advanced users.
    inline bool SendReport(void* data, size_t length) {
        return this->usb_hid.sendReport(0, data, (uint8_t)length);
    };

  protected:
    HID_JoystickReport_Data_t _report;
    uint32_t startMillis;
    Adafruit_USBD_HID usb_hid;
};

TUJoystick::TUJoystick(void)
{
  this->usb_hid.setPollInterval(1);
  this->usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  // setStringDescriptor is undefined on nRF52840
  // this->usb_hid.setStringDescriptor("TinyUSB Joystick");
}

void TUJoystick::begin(void)
{
  this->usb_hid.begin();

  // release all buttons, center all sticks, etc.
  end();
  startMillis = millis();
}

void TUJoystick::loop(void)
{
  if (startMillis != millis()) {
    write();
    startMillis = millis();
  }
}

void TUJoystick::end(void)
{
  memset(&_report, 0x00, sizeof(_report));
  SendReport(&_report, sizeof(_report));
}

void TUJoystick::write(void)
{
  SendReport(&_report, sizeof(_report));
}

void TUJoystick::write(void *report)
{
  memcpy(&_report, report, sizeof(_report));
  SendReport(&_report, sizeof(_report));
}

void TUJoystick::press(uint8_t b)
{
  b &= 0x7; // Limit value between 0..7
  _report.buttons |= (uint8_t)1 << b;
}


void TUJoystick::release(uint8_t b)
{
  b &= 0x7; // Limit value between 0..7
  _report.buttons &= ~((uint8_t)1 << b);
}


void TUJoystick::releaseAll(void)
{
  _report.buttons = 0;
}

void TUJoystick::buttons(uint8_t b)
{
  _report.buttons = b;
}


void TUJoystick::xAxis(uint8_t a)
{
  _report.xAxis = a;
}


void TUJoystick::yAxis(uint8_t a)
{
  _report.yAxis = a;
}
