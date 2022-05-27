#pragma once

#include <SPI.h>
#include <WiFiNINA.h>

typedef struct {
	int totalsize;
	int code;
	int keysize;
	int datasize;
} kvs_header_t;
#define kvs_message_key(h) (((char*)h) + sizeof(kvs_header_t))
#define kvs_message_data(h) (((char*)h) + sizeof(kvs_header_t) + h->keysize)
enum { KVS_NULL = 0, KVS_PUT, KVS_GET, KVS_PUT_REPLY, KVS_GET_REPLY } kvs_code_t;

class SSLClient {
 public:
	SSLClient();
	void init();
	void send(const char* message);
	void checkConnection();

 private:
	char* ssid;
	char* pass;
	const int wifi_timeout = 50000;
	char* server_url;
	int server_port;
	char* ssl_cert_file = SSL_CERT_FILE;
	char* ssl_key_file = SSL_KEY_FILE;
	uint8_t status = WL_IDLE_STATUS;
  WiFiSSLClient wifi_ssl_client;

	kvs_header_t* kvsMallocMessage(int code, int keysize, int datasize);
	kvs_header_t* kvsMallocPut(const void* key, int keysize, const void* data, int datasize);
	void put(WiFiSSLClient* ssl_client, const char* k, const char* v);
	void printMessage(char* n, kvs_header_t* message);
	void printCurrentNet();
	void printWifiData();
	void printMacAddress(byte mac[]);
	void listNetworks();
	void printEncryptionType(int thisType);
	const char* wifiStatusToString(uint8_t status);
	uint8_t beginEnterprise(const char* ssid, const char* username, const char* password, const char* identity,
	                        const char* ca);
	void ping(const char* host_name);
};
