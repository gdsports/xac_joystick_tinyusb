#!/usb/bin/python3
"""

Read keyboard and mouse input events then trigger joystick buttons and sticks.
Input from tty devices such as UARTs and ssh sessions do not count as keyboard
events. This works for USB and Bluetooth HID keyboards and mice.

To BT pair with keyboard or mouse, run the Raspbery Pi OS GUI and use the
Bluetooth tray applet to Add Device.

# Install input event module
sudo apt install python3-evdev

cd python
# Read keyboard and mouse events then press/release joystick buttons.
python3 joystick_inputevent.py

"""

import sys
import asyncio
import serial
import evdev
from JoystickUART import *

Joystick = JoystickUART()
try:
    Joystick.begin(serial.Serial('/dev/ttyS0', 115200, timeout=0))
except:
    print("Cannot open /dev/ttyS0")
    sys.exit(1)

# Map keyboard keys or mouse buttons to joystick buttons.
EVENT2BUTTON = {
    str(evdev.ecodes.KEY_A): 0,
    str(evdev.ecodes.KEY_B): 1,
    str(evdev.ecodes.KEY_X): 2,
    str(evdev.ecodes.KEY_Y): 3,
    str(evdev.ecodes.BTN_LEFT): 4,
    str(evdev.ecodes.BTN_RIGHT): 5,
    str(evdev.ecodes.BTN_MIDDLE): 6,
    str(evdev.ecodes.BTN_SIDE): 7,
}

async def handle_events(device):
    # Grab exclusive access means the shell and/or GUI no longer receives the input events
    with device.grab_context():
        async for event in device.async_read_loop():
            if event.code == evdev.ecodes.KEY_PAUSE:
                sys.exit(0)
            if str(event.code) in EVENT2BUTTON:
                joystick_button = EVENT2BUTTON[str(event.code)]
                if event.value == 1:
                    print('Key or button down', 'joystick down', joystick_button)
                    Joystick.press(joystick_button)
                elif event.value == 0:
                    print('Key or button up', 'joystick up', joystick_button)
                    Joystick.release(joystick_button)
            else:
                """ Map mouse motion to thumbstick motion """
                if event.code == evdev.ecodes.REL_X:
                    print('REL_X', event.value)
                    #Joystick.xAxis(?)
                elif event.code == evdev.ecodes.REL_Y:
                    print('REL_Y', event.value)
                    #Joystick.yAxis(?)
                elif event.code == evdev.ecodes.REL_WHEEL:
                    print('REL_WHEEL', event.value)
                    #Joystick.xAxis(?)
                elif event.code == evdev.ecodes.REL_HWHEEL:
                    print('REL_HWHEEL', event.value)
                    #Joystick.yAxis(?)
                elif event.code == evdev.ecodes.ABS_X:
                    print('ABS_X', event.value)
                    #Joystick.xAxis(?)
                elif event.code == evdev.ecodes.ABS_Y:
                    #Joystick.yAxis(?)
                    print('ABS_Y', event.value)

# Examine all input devices and find keyboards and mice.
# Process all keyboard and mouse input events.
for devpath in evdev.list_devices():
    device = evdev.InputDevice(devpath)
    print(device)
    print(device.path, device.name, device.phys)
    print(device.capabilities(verbose=True))
    if evdev.ecodes.EV_KEY in device.capabilities():
        print('Has EV_KEY')
        print(device.capabilities()[evdev.ecodes.EV_KEY])
        if evdev.ecodes.KEY_A in device.capabilities()[evdev.ecodes.EV_KEY]:
            print('Keyboard', device)
            asyncio.ensure_future(handle_events(device))
        elif evdev.ecodes.BTN_MOUSE in device.capabilities()[evdev.ecodes.EV_KEY]:
            print('Mouse', device)
            asyncio.ensure_future(handle_events(device))

loop = asyncio.get_event_loop()
loop.run_forever()
