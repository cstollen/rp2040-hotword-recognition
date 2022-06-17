#include "ssl_client.h"
#include <Arduino.h>

SSLClient::SSLClient() {
	ssid = WIFI_SSID;  // WPA2 network SSID
	pass = WIFI_PASS;  // WPA2 password
	server_url = SERVER_URL;
	server_port = SERVER_PORT;
	ssl_cert_file = SSL_CERT_FILE;
	ssl_key_file = SSL_KEY_FILE;
  init();
}

void SSLClient::init() {
	// check for the WiFi module:
	if (WiFi.status() == WL_NO_MODULE) {
		Serial.println("Communication with WiFi module failed!");
		// don't continue
		while (true)
			;
	}

	String fv = WiFi.firmwareVersion();
	if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
		Serial.println("Please upgrade the firmware");
	}
	WiFi.setTimeout(wifi_timeout);

	// Attempt to connect to WiFi network:
	while (status != WL_CONNECTED) {
		Serial.print("Attempting to connect to SSID: ");
		Serial.println(ssid);
		// Connect to WPA2 network
		WiFi.begin(ssid, pass);
		// Connect to WPA2 enterprise network
		// status = WiFi.beginEnterprise(ssid, user, pass);
		// status = WiFi.beginEnterprise(ssid, user, pass, identity, caCert);

		status = WiFi.status();

		if (status != WL_CONNECTED) {
			Serial.print("Reason code: ");
			Serial.println(WiFi.reasonCode());
			Serial.print("WiFi status: ");
			Serial.println(wifiStatusToString(status));
			delay(10000);
		}
	}

	Serial.println("Connected to the network");
	printCurrentNet();
	printWifiData();

	// Debug ping server
	// Serial.println("Attempting to connect to server");
	// IPAddress server_ip;
	// int err = WiFi.hostByName(server_url, server_ip);
	// if (err == 1) {
	// 	Serial.print("Domain: ");
	// 	Serial.println(server_url);
	// 	Serial.print("IP: ");
	// 	Serial.println(server_ip);
	// } else {
	// 	Serial.print("Could not resolve ip for: ");
	// 	Serial.println(server_url);
	// }
	// ping(server_url);

	// Connect to SSL server
	Serial.print("Connecting to ssl server: ");
	Serial.print(server_url);
	Serial.print(":");
	Serial.println(server_port);

	while (!wifi_ssl_client.connectSSL(server_url, server_port)) {
		Serial.print("Could not connect to ");
		Serial.print(server_url);
		Serial.print(":");
		Serial.println(server_port);
		delay(2000);
	}
	Serial.print("Connected to ");
	Serial.print(server_url);
	Serial.print(":");
	Serial.println(server_port);
}

void SSLClient::send(const char* message) {
	// Serial.println(message);
  put(&wifi_ssl_client, "hotword", message);
}

kvs_header_t* SSLClient::kvsMallocMessage(int code, int keysize, int datasize) {
	int totalsize = sizeof(kvs_header_t) + keysize + datasize;
	kvs_header_t* header = (kvs_header_t*)malloc(totalsize);
	header->code = code;
	header->totalsize = totalsize;
	header->keysize = keysize;
	header->datasize = datasize;
	return header;
}

kvs_header_t* SSLClient::kvsMallocPut(const void* key, int keysize, const void* data, int datasize) {
	kvs_header_t* message = kvsMallocMessage(KVS_PUT, keysize, datasize);
	memcpy(kvs_message_key(message), key, keysize);
	memcpy(kvs_message_data(message), data, datasize);
	return message;
}

void SSLClient::put(WiFiSSLClient* ssl_client, const char* k, const char* v) {
	kvs_header_t* request = kvsMallocPut(k, strlen(k) + 1, v, strlen(v) + 1);
	uint8_t* request_bytes = (uint8_t*)request;
	ssl_client->write(request_bytes, request->totalsize);
	free(request);
	kvs_header_t header;
	uint8_t* header_bytes = (uint8_t*)&header;
	ssl_client->read(header_bytes, sizeof(header));
	// should check reply...
}

void SSLClient::printMessage(char* n, kvs_header_t* message) {
	printf("%s [code: %d, ", n, message->code);
	printf("total: %d, ", message->totalsize);
	printf("key: %.*s (%d), ", message->keysize, kvs_message_key(message), message->keysize);
	printf("data: %.*s (%d)]\n", message->datasize, kvs_message_data(message), message->datasize);
}

void SSLClient::printCurrentNet() {
	// print the SSID of the network you're attached to:
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());

	// print the MAC address of the router you're attached to:
	byte bssid[6];
	WiFi.BSSID(bssid);
	Serial.print("BSSID: ");
	printMacAddress(bssid);

	// print the received signal strength:
	long rssi = WiFi.RSSI();
	Serial.print("Signal strength (RSSI): ");
	Serial.println(rssi);

	// print the encryption type:
	byte encryption = WiFi.encryptionType();
	Serial.print("Encryption Type: ");
	Serial.println(encryption, HEX);
}

void SSLClient::printWifiData() {
	// print your board's IP address:
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);

	// print your MAC address:
	byte mac[6];
	WiFi.macAddress(mac);
	Serial.print("MAC address: ");
	printMacAddress(mac);
}

void SSLClient::printMacAddress(byte mac[]) {
	for (int i = 5; i >= 0; i--) {
		if (mac[i] < 16) {
			Serial.print("0");
		}
		Serial.print(mac[i], HEX);
		if (i > 0) {
			Serial.print(":");
		}
	}
	Serial.println();
}

void SSLClient::listNetworks() {
	// scan for nearby networks:
	Serial.println("Scan Networks");
	int numSsid = WiFi.scanNetworks();
	if (numSsid == -1) {
		Serial.println("Couldn't get a WiFi connection");
		while (true)
			;
	}

	// print the list of networks seen:
	Serial.print("Number of available networks:");
	Serial.println(numSsid);

	// print the network number and name for each network found:
	for (int thisNet = 0; thisNet < numSsid; thisNet++) {
		Serial.print(thisNet);
		Serial.print(") ");
		Serial.print(WiFi.SSID(thisNet));
		Serial.print("\tSignal: ");
		Serial.print(WiFi.RSSI(thisNet));
		Serial.print(" dBm");
		Serial.print("\tEncryption: ");
		printEncryptionType(WiFi.encryptionType(thisNet));
	}
}

void SSLClient::printEncryptionType(int thisType) {
	// read the encryption type and print out the name:
	switch (thisType) {
		case ENC_TYPE_WEP:
			Serial.println("WEP");
			break;
		case ENC_TYPE_TKIP:
			Serial.println("WPA");
			break;
		case ENC_TYPE_CCMP:
			Serial.println("WPA2");
			break;
		case ENC_TYPE_NONE:
			Serial.println("None");
			break;
		case ENC_TYPE_AUTO:
			Serial.println("Auto");
			break;
		case ENC_TYPE_UNKNOWN:
		default:
			Serial.println("Unknown");
			break;
	}
}

const char* SSLClient::wifiStatusToString(uint8_t status) {
	// WL_CONNECTED: assigned when connected to a WiFi network;
	// WL_AP_CONNECTED : assigned when a device is connected in Access Point mode;
	// WL_AP_LISTENING : assigned when the listening for connections in Access Point mode;
	// WL_NO_SHIELD: assigned when no WiFi shield is present;
	// WL_NO_MODULE: assigned when the communication with an integrated WiFi module fails;
	// WL_IDLE_STATUS: it is a temporary status assigned when WiFi.begin() is called and remains active until the number
	// of attempts expires (resulting in WL_CONNECT_FAILED) or a connection is established (resulting in WL_CONNECTED);
	// WL_NO_SSID_AVAIL: assigned when no SSID are available;
	// WL_SCAN_COMPLETED: assigned when the scan networks is completed;
	// WL_CONNECT_FAILED: assigned when the connection fails for all the attempts;
	// WL_CONNECTION_LOST: assigned when the connection is lost;
	// WL_DISCONNECTED: assigned when disconnected from a network;
	switch (status) {
		case WL_IDLE_STATUS:
			return "WL_IDLE_STATUS";
		case WL_NO_SSID_AVAIL:
			return "WL_NO_SSID_AVAIL";
		case WL_SCAN_COMPLETED:
			return "WL_SCAN_COMPLETED";
		case WL_CONNECTED:
			return "WL_CONNECTED";
		case WL_CONNECT_FAILED:
			return "WL_CONNECT_FAILED";
		case WL_CONNECTION_LOST:
			return "WL_CONNECTION_LOST";
		case WL_DISCONNECTED:
			return "WL_DISCONNECTED";
		case WL_AP_LISTENING:
			return "WL_AP_LISTENING";
		case WL_AP_CONNECTED:
			return "WL_AP_CONNECTED";
		case WL_NO_MODULE:
			return "WL_NO_MODULE";
	}
}

uint8_t SSLClient::beginEnterprise(const char* ssid, const char* username, const char* password, const char* identity,
                                   const char* ca) {
	uint8_t status = WL_IDLE_STATUS;

	// set passphrase
	if (WiFiDrv::wifiSetEnterprise(0 /*PEAP/MSCHAPv2*/, ssid, strlen(ssid), username, strlen(username), password,
	                               strlen(password), identity, strlen(identity), ca, strlen(ca) + 1) != WL_FAILURE) {
		for (unsigned long start = millis(); (millis() - start) < wifi_timeout;) {
			delay(WL_DELAY_START_CONNECTION);
			status = WiFiDrv::getConnectionStatus();
			if ((status != WL_IDLE_STATUS) && (status != WL_NO_SSID_AVAIL) && (status != WL_SCAN_COMPLETED)) {
				break;
			}
		}
	} else {
		status = WL_CONNECT_FAILED;
	}
	return status;
}

void SSLClient::ping(const char* host_name) {
	Serial.print("Pinging ");
	Serial.print(host_name);
	Serial.print(": ");

	int ping_result = WiFi.ping(host_name);
	if (ping_result >= 0) {
		Serial.print("SUCCESS! RTT = ");
		Serial.print(ping_result);
		Serial.println(" ms");
	} else {
		Serial.print("FAILED! Error code: ");
		Serial.println(ping_result);
	}
	Serial.println();

	delay(1000);
}

void SSLClient::checkConnection() {
	if (!wifi_ssl_client.connected()) {
		wifi_ssl_client.stop();
		Serial.println();
		Serial.println("Disconnected from server");
	}
}