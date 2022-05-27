#ifndef MYSSL_H
#define MYSSL_H

#include <openssl/ssl.h>

typedef struct {
	SSL_CTX *ctx;
	int sock;
	SSL *ssl;
	int client;
} myssl_handler_t;

myssl_handler_t *myssl_server_handler(int port);
myssl_handler_t *myssl_client_handler();

int myssl_accept(myssl_handler_t *h);
void myssl_connect(myssl_handler_t *h, char *server, int port);
void myssl_close(myssl_handler_t *h);
void myssl_free_handler(myssl_handler_t *h);

int myssl_read(myssl_handler_t *h, void *ptr, int size);
void myssl_write(myssl_handler_t *h, const void *ptr, int size);

#endif