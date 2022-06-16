#! /bin/bash

if [[ ! -f "config-template.ini" ]]; then
  echo "Execute config script from project root folder containing config-template.ini"
  exit 1
fi

if [[ ! -f "config.ini" ]]; then
	cp "config-template.ini" "config.ini"
  echo "Empty config.ini created"
fi

source "config.ini"

if [[ "$WIFI_SSID" == "" ]] || [[ "$WIFI_PASS" == "" ]] || [[ "$SERVER_URL" == "" ]]; then
	echo "Please set configuration variables in config.ini"
	exit 1
fi

if [[ ! -f "tools/arduino-fwuploader/arduino-fwuploader" ]]; then
	echo "Arduino firmware updater not found. Please run 'install-dependencies.sh'"
	exit 1
fi

if [[ ! -d "cert" ]]; then
	mkdir "cert"
fi
cd cert
if [[ ! -f "cert.pem" ]] || [[ ! -f "key.pem" ]]; then
	echo "Generating SSL certificate and key"
	openssl req -nodes -new -x509 -days 3650 -extensions v3_ca -keyout key.pem -out cert.pem -subj /CN=$SERVER_URL
	cd ..
	echo ""
	echo "Uploading certificate to microcontroller"
	tools/arduino-fwuploader/arduino-fwuploader certificates flash --file cert/cert.pem -b arduino:mbed_nano:nanorp2040connect -a /dev/ttyACM0
fi
cd ..

echo "Configuration"
echo "WiFi SSID: $WIFI_SSID"
echo "Server URL: $SERVER_URL"
echo "Server port: $SERVER_PORT"

