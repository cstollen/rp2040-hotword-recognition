#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "panic.h"
#include "myssl.h"

static int create_socket() {
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	assert_panic(sock != -1, "unable to create socket");
	return sock;
}

static void bind_socket(int sock, int port) {
	int optval = 1;
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	int ss = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	assert_panic(ss != -1, "setsockopt(SO_REUSEADDR) failed");

	int b = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
	assert_panic(b != -1, "unable to bind");

	int l = listen(sock, 1);
	assert_panic(l != -1, "unable to listen");
}

static SSL_CTX *create_context_server() {
	SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
	assert_panic(ctx != 0, "unable to create SSL server context");
	int uc = SSL_CTX_use_certificate_chain_file(ctx, SSL_CERT_FILE);
	assert_panic(uc == 1, "unable to use cert.pem");
	int up = SSL_CTX_use_PrivateKey_file(ctx, SSL_KEY_FILE, SSL_FILETYPE_PEM);
	assert_panic(up == 1, "unable to use key.pem");
	return ctx;
}

static SSL_CTX *create_context_client() {
	SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
	assert_panic(ctx != 0, "unable to create SSL client context");
	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
	int lv = SSL_CTX_load_verify_locations(ctx, SSL_CERT_FILE, NULL);
	assert_panic(lv == 1, "cannot verify cert.pem");
	return ctx;
}

myssl_handler_t *myssl_server_handler(int port) {
	myssl_handler_t *h = malloc(sizeof(myssl_handler_t));
	panic_malloc(h);

	h->ctx = create_context_server();  // server
	h->sock = create_socket();
	bind_socket(h->sock, port);
	h->ssl = 0;
	h->client = -1;
	return h;
}

myssl_handler_t *myssl_client_handler() {
	myssl_handler_t *h = malloc(sizeof(myssl_handler_t));
	panic_malloc(h);

	h->ctx = create_context_client();  // client
	h->ssl = 0;
	h->sock = -1;
	h->client = -1;
	return h;
}

int myssl_accept(myssl_handler_t *h) {
	struct sockaddr_in addr;
	unsigned int len = sizeof(addr);

	h->client = accept(h->sock, (struct sockaddr *)&addr, &len);
	assert_panic(h->client != -1, "unable to accept");
	h->ssl = SSL_new(h->ctx);
	SSL_set_fd(h->ssl, h->client);
	return SSL_accept(h->ssl);
}

void myssl_connect(myssl_handler_t *h, char *server, int port) {
	struct sockaddr_in addr;
	inet_pton(AF_INET, server, &addr.sin_addr.s_addr);
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;

	h->client = create_socket();
	int c = connect(h->client, (struct sockaddr *)&addr, sizeof(addr));
	assert_panic(c != -1, "unable to TCP connect to server");

	h->ssl = SSL_new(h->ctx);
	SSL_set_fd(h->ssl, h->client);
	c = SSL_connect(h->ssl);
	assert_panic(c == 1, "unable to SSL connect to server");
}

void myssl_write(myssl_handler_t *h, const void *ptr, int size) { SSL_write(h->ssl, ptr, size); }

int myssl_read(myssl_handler_t *h, void *ptr, int size) { return SSL_read(h->ssl, ptr, size); }

void myssl_close(myssl_handler_t *h) {
	SSL_shutdown(h->ssl);
	SSL_free(h->ssl);
	if (h->client > 0) close(h->client);
}

void myssl_free_handler(myssl_handler_t *h) {
	if (h->sock > 0) close(h->sock);
	SSL_CTX_free(h->ctx);
	free(h);
}
