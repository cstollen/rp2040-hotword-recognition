#include <stdlib.h>
#include <string.h>

#include "kvs.h"
#include "panic.h"

enum { KVS_NULL = 0, KVS_PUT, KVS_GET, KVS_PUT_REPLY, KVS_GET_REPLY } kvs_code_t;

kvs_header_t *kvs_malloc_message(int code, int keysize, int datasize) {
	int totalsize = sizeof(kvs_header_t) + keysize + datasize;
	kvs_header_t *header = (kvs_header_t *)malloc(totalsize);
	panic_malloc(header);
	header->code = code;
	header->totalsize = totalsize;
	header->keysize = keysize;
	header->datasize = datasize;
	return header;
}

void print_message(char *n, kvs_header_t *message) {
	printf("%s [code: %d, ", n, message->code);
	printf("total: %d, ", message->totalsize);
	printf("key: %.*s (%d), ", message->keysize, kvs_message_key(message), message->keysize);
	printf("data: %.*s (%d)]\n", message->datasize, kvs_message_data(message), message->datasize);
}

void print_banner_message(kvs_header_t *message) {
	char cmd_buffer[20];
	sprintf(cmd_buffer, "figlet %s", kvs_message_data(message));
	int cmd_result = system(cmd_buffer);
}

kvs_header_t *kvs_process(void *message, hash_t *hash) {
	kvs_header_t *request = (kvs_header_t *)message;
	kvs_header_t *reply = 0;
	int size;
	char *data;
	// print_message("REQUEST", request);
	switch (request->code) {
		case KVS_PUT:
			hash_put(hash, kvs_message_key(request), request->keysize, kvs_message_data(request), request->datasize);
			reply = kvs_malloc_message(KVS_PUT_REPLY, 0, 0);
			break;
		case KVS_GET:
			data = hash_get(hash, kvs_message_key(request), request->keysize, &size);
			reply = kvs_malloc_message(KVS_PUT_REPLY, 0, size);
			if (data) memcpy(kvs_message_data(reply), data, size);
			break;
		default:
			break;
	}
	// print_message("REPLY", reply);
	return reply;
}

kvs_header_t *kvs_malloc_put(void *key, int keysize, void *data, int datasize) {
	kvs_header_t *message = kvs_malloc_message(KVS_PUT, keysize, datasize);
	memcpy(kvs_message_key(message), key, keysize);
	memcpy(kvs_message_data(message), data, datasize);
	return message;
}

kvs_header_t *kvs_malloc_get(void *key, int keysize) {
	kvs_header_t *message = kvs_malloc_message(KVS_GET, keysize, 0);
	memcpy(kvs_message_key(message), key, keysize);
	return message;
}
