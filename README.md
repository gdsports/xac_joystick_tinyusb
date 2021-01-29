# XAC Joystick Library for Adafruit TinyUSB

Be sure to set the USB Stack to TinyUSB.

The example works on Adafruit SAMD (tested with an ItsyBitsy M0), Seeeduino
SAMD (tested on XIAO), and Adafruit Feather nRF52840 Express. Verified to
work on an Xbox Adaptive Controller (XAC).

This produces a USB composite device with HID joystick and CDC ACM port.
Removing the CDC ACM port is not required.

If the CDC ACM port must be removed, the changes are described
[here](https://github.com/adafruit/Adafruit_TinyUSB_ArduinoCore/issues/19).
Patching is the only option for now.

Similar patches are needed if using the Seeeduino SAMD or the Adafruit nRF52
boards.
