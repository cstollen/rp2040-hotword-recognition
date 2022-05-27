#! /bin/bash

# Configure
echo ""
./configure.sh

# Build SSL Server
echo ""
echo "Building SSL Server"
cd ssl-server
if [ ! -d "build" ]; then
  mkdir build
fi
cd build
cmake ..
make -j$(nproc)

# Build and upload RP2040 Client
echo ""
echo "Building and uploading RP2040 Client"
cd ../../rp2040-hotword-recognition-client
pio run --target upload

