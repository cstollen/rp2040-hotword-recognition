#ifndef KVS_H_
#define KVS_H_

#include "hash.h"

#define KVS_SERVER_PORT SERVER_PORT

typedef struct {
	int totalsize;
	int code;
	int keysize;
	int datasize;
} kvs_header_t;

kvs_header_t *kvs_malloc_message(int code, int keysize, int datasize);
kvs_header_t *kvs_process(void *message, hash_t *hash);
kvs_header_t *kvs_malloc_put(void *key, int keysize, void *data, int datasize);
kvs_header_t *kvs_malloc_get(void *key, int keysize);
void print_message(char *n, kvs_header_t *message);
void print_banner_message(kvs_header_t *message);

#define kvs_message_key(h) (((char *)h) + sizeof(kvs_header_t))
#define kvs_message_data(h) (((char *)h) + sizeof(kvs_header_t) + h->keysize)
#endif
