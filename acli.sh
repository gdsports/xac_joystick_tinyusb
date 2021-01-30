#!/bin/bash
ARDDIR=${HOME}/Sync/acli_xac_joystick_tinyusb
export ARDUINO_BOARD_MANAGER_ADDITIONAL_URLS="https://adafruit.github.io/arduino-board-index/package_adafruit_index.json https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json"
export ARDUINO_DIRECTORIES_DATA="${ARDDIR}/data"
export ARDUINO_DIRECTORIES_DOWNLOADS="${ARDDIR}/downloads"
export ARDUINO_DIRECTORIES_USER="${ARDDIR}/user"
export MYPROJECT_EXAMPLES="${PWD}/examples"
export MYPROJECT_TOOLS="${PWD}/tools"
arduino-cli core update-index
mkdir -p ${ARDUINO_DIRECTORIES_DOWNLOADS}/packages
mkdir -p ${ARDUINO_DIRECTORIES_DOWNLOADS}/libraries
if [ -d ~/Sync/ard_staging ]
then
    cp -R ~/Sync/ard_staging/* ${ARDUINO_DIRECTORIES_DOWNLOADS}
fi
arduino-cli core install adafruit:samd
arduino-cli core install Seeeduino:samd
arduino-cli core install adafruit:nrf52
arduino-cli core list
arduino-cli lib update-index
arduino-cli lib install "Bounce2"
arduino-cli lib install "Adafruit DotStar"
arduino-cli lib install "Adafruit TinyUSB Library"
ln -s ${PWD} ${ARDUINO_DIRECTORIES_USER}/libraries
# Compile all examples for all boards
BOARDS=('Seeeduino:samd:seeed_XIAO_m0' 'adafruit:samd:adafruit_trinket_m0' 'adafruit:samd:adafruit_qtpy_m0' 'adafruit:samd:adafruit_itsybitsy_m0' 'adafruit:samd:adafruit_itsybitsy_m4' 'adafruit:nrf52:feather52840' 'adafruit:nrf52:itsybitsy52840')
for board in "${BOARDS[@]}" ; do
    export ARDUINO_BOARD_FQBN=${board}
    ARDUINO_BOARD_FQBN2=${ARDUINO_BOARD_FQBN//:/.}
    arduino-cli cache clean
    find ${MYPROJECT_EXAMPLES} -name '*.ino' -print0 | xargs -0 -n 1 arduino-cli compile --fqbn ${board} --verbose --build-properties build.vid=0x0f0d,build.pid=0x00c1,build.flags.usbstack=-DUSE_TINYUSB
    # Convert all HEX to UF2 for drag-and-drop burning on boards with UF2 boot loader
    for MYSKETCH in ${MYPROJECT_EXAMPLES}/* ; do
        if [ ! -d ${MYSKETCH}/build/${ARDUINO_BOARD_FQBN2} ] ; then
            mkdir -p ${MYSKETCH}/build/${ARDUINO_BOARD_FQBN2}
        fi
        pushd ${MYSKETCH}/build/${ARDUINO_BOARD_FQBN2}
        for i in *.bin ; do
            if [[ -f $i ]] ; then
                if [[ ${board} == *"m4"* ]] ; then
                    ${MYPROJECT_TOOLS}/uf2conv.py -c -b 0x4000 $i -o $i.uf2
                else
                    ${MYPROJECT_TOOLS}/uf2conv.py -c $i -o $i.uf2
                fi
            fi
        done
        FIRMWARE=${MYSKETCH}/firmware/${ARDUINO_BOARD_FQBN2}
        if [[ ! -d ${FIRMWARE} ]] ; then
            mkdir -p ${FIRMWARE}
        fi
        mv *.ino.bin.uf2 ${FIRMWARE}
        popd
    done
done >errors 2>&1
