#!/usr/bin/python3
"""
Interface to XAC joystick (joystick_uart.ino) via serial UART port.

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
"""
from struct import pack
import threading

class JoystickUART:
    """Joystick Serial Interface"""
    def __init__(self):
        self.thread_lock = threading.Lock()
        self.ser_port = 0
        self.x_axis = 128
        self.y_axis = 128
        self.my_buttons = 0

    def begin(self, serial_port):
        """Start JoystickUART"""
        with self.thread_lock:
            self.ser_port = serial_port
            self.x_axis = 128
            self.y_axis = 128
            self.my_buttons = 0
            self.write()
        return

    def end(self):
        """End JoystickUART"""
        self.ser_port.close()
        return

    def write(self):
        """Send JoystickUART state"""
        self.ser_port.write(pack('<BBBBBBBBBBBB', 2, 9, 2, self.my_buttons, \
            self.x_axis, self.y_axis, \
            0, 0, 0, 0, 0, 3))
        return

    def press(self, button_number):
        """Press button 0..7"""
        with self.thread_lock:
            button_number = button_number & 0x07
            self.my_buttons |= (1<<button_number)
            self.write()
        return

    def release(self, button_number):
        """Release button 0..7"""
        with self.thread_lock:
            button_number = button_number & 0x07
            self.my_buttons &= ~(1<<button_number)
            self.write()
        return

    def releaseAll(self):
        """Release all buttons"""
        with self.thread_lock:
            self.my_buttons = 0
            self.write()
        return

    def buttons(self, buttons):
        """Set all buttons"""
        with self.thread_lock:
            self.my_buttons = buttons
            self.write()
        return

    def xAxis(self, position):
        """Move left stick X axis 0..128..255"""
        with self.thread_lock:
            self.x_axis = position
            self.write()
        return

    def yAxis(self, position):
        """Move left stick Y axis 0..128..255"""
        with self.thread_lock:
            self.y_axis = position
            self.write()
        return

def main():
    """ test JoystickUART class """
    import sys
    import serial
    import time
    import random

    xac = JoystickUART()
    try:
        xac.begin(serial.Serial('/dev/ttyS0', 115200, timeout=0))
    except:
        print("Cannot open /dev/ttyS0")
        sys.exit(1)

    while True:
        # Press and hold every button 0..7
        for button in range(0, 8):
            xac.press(button)
            time.sleep(0.1)
        time.sleep(1)
        # Release all buttons
        xac.releaseAll()
        time.sleep(1)
        # Press all 8 buttons at the same time
        xac.buttons(0xff)
        time.sleep(1)
        # Release all buttons
        xac.releaseAll()
        time.sleep(1)
        xac.xAxis(random.randint(0, 255))
        xac.yAxis(random.randint(0, 255))

if __name__ == "__main__":
    main()
