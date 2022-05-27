Import("env")
import re
import os
import sys

config_file = os.path.realpath(os.path.join(os.getcwd(), "../config.ini"))

# read from ini file.
def iniRead(ini_file, key):
	value = None
	with open(ini_file, 'r') as f:
		for line in f:
			match = re.match(r'^ *' + key + ' *= *.*$', line, re.M | re.I)
			if match:
				value = match.group()
				value = re.sub(r'^ *' + key + ' *= *', '', value)
				break
	return value


#env["WIFI_SSID"] = "Caroline"
#env["WIFI_PASS"] = "citec12104"
#env["SERVER_URL"] = "hoegarden.techfak.uni-bielefeld.de"
#env["SERVER_PORT"] = 33412
env["WIFI_SSID"] = iniRead(config_file, "WIFI_SSID")
env["WIFI_PASS"] = iniRead(config_file, "WIFI_PASS")
env["SERVER_URL"] = iniRead(config_file, "SERVER_URL")
env["SERVER_PORT"] = iniRead(config_file, "SERVER_PORT")
#print(env.Dump())

