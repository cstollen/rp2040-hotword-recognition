#! /bin/bash

SERIAL_PORT=/dev/ttyACM0

if [ ! -f "rp2040-hotword-recognition-client/.pio/build/nanorp2040connect/firmware.elf.uf2" ]; then
  echo "Building project"
  build-all.sh
fi

cat $SERIAL_PORT

