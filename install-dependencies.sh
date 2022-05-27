#! /bin/bash

check_install_package () {
	REQUIRED_PKG="$1"
	PKG_OK=$(dpkg-query -W --showformat='${Status}\n' ${REQUIRED_PKG} |grep "install ok installed")
	echo "Checking for package $REQUIRED_PKG"
	if [ "" = "$PKG_OK" ]; then
		echo "Package $REQUIRED_PKG not found. Installing $REQUIRED_PKG."
		sudo apt-get --yes install $REQUIRED_PKG
	else
		echo "Package $REQUIRED_PKG found"
	fi
}

pio_cmd_path=$(which pio)
openssl_cmd_path=$(which openssl)
figlet_cmd_path=$(which figlet)
fwuploader_cmd_path="tools/arduino-fwuploader/arduino-fwuploader"

if [ $pio_cmd_path = "" ]; then
	echo "PlatformIO executable not found"
	echo "Installing PlatformIO"
	check_install_package "python3-pip"
	sudo pip3 install platformio
	sudo ~/.platformio/packages/framework-arduino-mbed/post_install.sh
else
	echo "Found PlatformIO executable: $pio_cmd_path"
fi

if [ $openssl_cmd_path = "" ]; then
	echo "OpenSSL executable not found"
	echo "Installing OpenSSL"
	check_install_package "libssl-dev"
else
	echo "Found OpenSSL executable: $openssl_cmd_path"
fi

if [ $figlet_cmd_path = "" ]; then
	echo "Figlet executable not found"
	echo "Installing figlet command line banner"
	check_install_package "figlet"
else
	echo "Found figlet executable: $figlet_cmd_path"
fi

if [ ! -f $fwuploader_cmd_path ]; then
	echo "Arduino firmware uploader executable not found"
	echo "Installing arduino firmware uploader into tools folder"
	if [ ! -d "tools" ]; then
  	mkdir tools
	fi
	if [ ! -f "tools/arduino-fwuploader/arduino-fwuploader_2.2.0_Linux_64bit.tar.gz" ]; then
		cd tools
		if [ ! -d "arduino-fwuploader" ]; then
			mkdir arduino-fwuploader
		fi
		cd arduino-fwuploader
		wget https://github.com/arduino/arduino-fwuploader/releases/download/2.2.0/arduino-fwuploader_2.2.0_Linux_64bit.tar.gz
		cd ../..
	fi
	if [ ! -f $fwuploader_cmd_path ]; then
		cd tools/arduino-fwuploader
  	tar -xzf arduino-fwuploader_2.2.0_Linux_64bit.tar.gz
  	cd ../..
	fi
else
	echo "Found arduino firmware uploader executable: $fwuploader_cmd_path"
fi

