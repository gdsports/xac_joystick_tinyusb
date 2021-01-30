#!/bin/bash
IDEVER="1.8.13"
WORKDIR="${HOME}/Sync/autobuild_xac_joystick_tinyusb"
mkdir -p ${WORKDIR}
# Install Ardino IDE in work directory
if [ -f ~/Downloads/arduino-${IDEVER}-linux64.tar.xz ]
then
    tar xf ~/Downloads/arduino-${IDEVER}-linux64.tar.xz -C ${WORKDIR}
else
    wget -O arduino.tar.xz https://downloads.arduino.cc/arduino-${IDEVER}-linux64.tar.xz
    tar xf arduino.tar.xz -C ${WORKDIR}
    rm arduino.tar.xz
fi
# Create portable sketchbook and library directories
IDEDIR="${WORKDIR}/arduino-${IDEVER}"
LIBDIR="${IDEDIR}/portable/sketchbook/libraries"
mkdir -p "${LIBDIR}"
export PATH="${IDEDIR}:${PATH}"
cd ${IDEDIR}
which arduino
# Install board package
if [ -d ~/Sync/ard_staging ]
then
    cp -R ~/Sync/ard_staging/* ${IDEDIR}/portable/staging/
fi
arduino --pref "compiler.warning_level=default" --save-prefs
arduino --pref "boardsmanager.additional.urls=https://adafruit.github.io/arduino-board-index/package_adafruit_index.json,https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json" --save-prefs
arduino --install-boards "adafruit:samd"
arduino --install-boards "Seeeduino:samd"
arduino --install-boards "adafruit:nrf52"
BOARD="adafruit:samd:adafruit_itsybitsy_m0"
arduino --board "${BOARD}" --save-prefs
arduino --pref "custom_usbstack=adafruit_itsybitsy_m0_tinyusb" --save-prefs
CC="arduino --verify --board ${BOARD}"
# Button debouncer
arduino --install-library "Bounce2"
arduino --install-library "Adafruit DotStar"
arduino --install-library "Adafruit TinyUSB Library"
# Arduino SAMD
#arduino --install-boards "arduino:samd"
# Arduino Nano 33 BLE
#arduino --install-boards "arduino:mbed"
#arduino --install-library "ArduinoBLE"
#https://github.com/dmpolukhin/Adafruit_nRF52_Arduino
git init
echo -e "*.gz\n*.bz2\n*.tgz\n*.zip" >.gitignore
git add .
git commit -m "First draft"
ln -s ~/Sync/xac_joystick_tinyusb ${LIBDIR}
