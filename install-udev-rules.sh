#! /bin/bash

# Copy udev-rules/99-platformio-udev.rules to /etc/udev/rules.d
if [[ ! -f "/etc/udev/rules.d/99-platformio-udev.rules" ]]; then
  sudo cp udev-rules/99-platformio-udev.rules /etc/udev/rules.d
  echo "udev-rules/99-platformio-udev.rules installed"

  # Reload udev rules
  sudo udevadm control --reload-rules
  sudo udevadm trigger
else
  echo "udev-rules/99-platformio-udev.rules already installed"
fi

