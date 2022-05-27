#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hash.h"

static void panic_malloc(void *ptr) {
	if (!ptr) {
		fprintf(stderr, "Panic: hash cannot malloc!");
		exit(1);
	}
}

hash_t *hash_create() {
	hash_t *h = (hash_t *)malloc(sizeof(hash_t));
	panic_malloc(h);
	h->size = HASH_SIZE;
	for (int i = 0; i < h->size; i++) h->nodes[i] = 0;
	return h;
}

static void display_data(void *p, int size, FILE *out) {
	// maybe print hexadecimal ???
	fwrite(p, size, 1, out);
}

void hash_display(hash_t *h, FILE *out) {
	for (int i = 0; i < h->size; i++) {
		fprintf(out, "index %d:\n", i);
		_hash_node_t *node = h->nodes[i];
		while (node) {
			fprintf(out, "key: ");
			display_data(node->key, node->keysize, out);
			fprintf(out, " value: ");
			display_data(node->data, node->datasize, out);
			fprintf(out, "\n");
			node = node->next;
		}
	}
}

static int hash_index(hash_t *h, void *key, int keysize) {
	int sum = 0;
	for (int i = 0; i < keysize; i++) sum += ((unsigned char *)key)[i];
	return sum % h->size;
}

static int compare_key(void *key, int keysize, void *skey, int skeysize) {
	char *ckey = (char *)key;
	char *cskey = (char *)skey;

	if (keysize != skeysize) return 0;
	for (int i = 0; i < keysize; i++)
		if (ckey[i] != cskey[i]) return 0;
	return 1;
}

_hash_node_t **find_nodeptr(hash_t *h, void *key, int keysize) {
	int index = hash_index(h, key, keysize);
	_hash_node_t **nodeptr = h->nodes + index;
	_hash_node_t *node = *nodeptr;
	while (node) {
		if (compare_key(key, keysize, node->key, node->keysize)) return nodeptr;
		nodeptr = &(node->next);
		node = *nodeptr;
	}
	return nodeptr;
}

void *hash_get(hash_t *h, void *key, int keysize, int *datasize) {
	_hash_node_t **nodeptr = find_nodeptr(h, key, keysize);
	if (*nodeptr) {
		_hash_node_t *node = *nodeptr;
		if (datasize) *datasize = node->datasize;
		return node->data;
	}
	return 0;
}

void hash_put(hash_t *h, void *key, int keysize, void *data, int datasize) {
	_hash_node_t **nodeptr = find_nodeptr(h, key, keysize);
	_hash_node_t *node = *nodeptr;
	if (node) {
		if (node->datasize < datasize) {
			free(node->data);
			node->data = 0;
		}
	} else {
		node = (_hash_node_t *)malloc(sizeof(_hash_node_t));
		panic_malloc(node);
		node->key = malloc(keysize);
		node->keysize = keysize;
		panic_malloc(node->key);
		memcpy(node->key, key, keysize);
		node->data = 0;
	}
	if (!node->data) {
		node->data = malloc(datasize);
		node->datasize = datasize;
		panic_malloc(node->data);
	}

	memcpy(node->data, data, datasize);
	node->next = 0;
	*nodeptr = node;
}
