# XAC Joystick Gadget

Emulate an XAC compatible joystick on the USB port. Receive joystick event input
on Serial1 from another computer. Works on Adafruit Trinket M0 and other SAMD21
boards.

The data format in the serial port looks like this.

```
<STX> <length> <type> 8 bytes of binary data <ETX>
0x02    0x09    0x02                          0x03
```

The type least significant 4 bits should be 2 for joysticks. The 4 most
significant bits specify an index in case multiple gadgets are on the the same
bus. For example, gadget 0 ignores packets with the index !=0.  gadget 1
ignores packets with the index != 1.

```
8 buttons             Bits 0..7
analog thumbstick     X=0..128..255, Y=0..128..255

struct JoystickEventData
{
  uint8_t   buttons;
  uint8_t   xAxis;
  uint8_t   yAxis;
  uint8_t[5] filler;
}__attribute__((packed));
```

## Building the firmware

See xac_joystick_tinyusb/README.md for details on building the firmware using
the Arduino IDE. See acli.sh for an automatic build script using Linux and the
Arduino CLI.

## Using the Gadget

To use the gadget with a computer such as a Raspberry Pi, connect the Trinket
M0 TX/RX to RX/TX of a CP2104 USB serial adaptor.

[CP2104 breakout board](https://www.adafruit.com/product/3309)
[Trinket M0](https://www.adafruit.com/product/3500)

Trinket M0  |CP2104
------------|---------
Gnd         |GND
TX(4)       |RXD
RX(3)       |TXD
Bat         |5V

Alternatively connect the Trinket M0 to a Pi UART.

Trinket M0  |Pi UART
------------|---------
Gnd         |GND
RX(3)       |UART TX(14)
Bat         |5V


* python/JoystickUART.py

Class that handles formatting data to send over USB serial to the Trinket M0.
It includes a self-test program that assumes a Trinket M0 running this example
on /dev/ttyUSB0. Run it like this.

```
python3 JoystickUART.py
```

## Enabling the mini UART (primary) on Pi Zero W

Run raspi-config
Interface Options
  P6 Serial Port
    login shell? No
    enable serial port hardware? Yes

sudo reboot

The serial port is named /dev/ttyS0 and UART Tx is on Pi GPIO 14. This connects
the Trinket M0 RX pin 3. See the previous section for a table.

This could be used to connect to both USB ports on an Xbox Adaptive Controller.

```
Pi Zero W -> Left USB -> XAC <- Right USB <- Trinket M0
   UART Tx                                     UART Rx
     |                                           |
     +-------------------------------------------+
```
