#ifndef _HASH_H
#define _HASH_H

#include <stdio.h>

#define HASH_SIZE 7

typedef struct _hash_node_t {
	void *key;
	int keysize;
	void *data;
	int datasize;
	struct _hash_node_t *next;
} _hash_node_t;

typedef struct {
	int size;
	_hash_node_t *nodes[HASH_SIZE];
} hash_t;

hash_t *hash_create();
void hash_put(hash_t *h, void *key, int keysize, void *data, int datasize);
void *hash_get(hash_t *h, void *key, int keysize, int *datasize);
void hash_display(hash_t *h, FILE *out);

#endif
