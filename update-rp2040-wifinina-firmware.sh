#! /bin/bash

if [[ ! -f "tools/arduino-fwuploader/arduino-fwuploader" ]]; then
	echo "Arduino firmware updater not found. Please run 'install-dependencies.sh'"
	exit 1
fi

echo "Updating Arduino RP2040 Nano Connect WifiNINA module to firmware version 1.4.8"
tools/arduino-fwuploader/arduino-fwuploader firmware flash -b arduino:mbed_nano:nanorp2040connect -a /dev/ttyACM0 -m NINA@1.4.8

