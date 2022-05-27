#include <stdio.h>
#include <string.h>

#include "myssl.h"
#include "kvs.h"
#include "hash.h"
#include "panic.h"

int main(int argc, char **argv) {
	hash_t *hash = hash_create();

	myssl_handler_t *handler = myssl_server_handler(KVS_SERVER_PORT);

	printf("Server accepting connection on port %d\n", KVS_SERVER_PORT);
	int a = myssl_accept(handler);
	assert_panic(a >= 0, "Server unable to accept");

	printf("Server accepted client\n");

	while (1) {
		kvs_header_t header;
		myssl_read(handler, &header, sizeof(header));
		char *message = (char *)malloc(header.totalsize);
		panic_malloc(message);

		memcpy(message, &header, sizeof(header));
		myssl_read(handler, message + sizeof(header), header.totalsize - sizeof(header));

		kvs_header_t *reply = kvs_process(message, hash);

		// print_message("REQUEST", (kvs_header_t *)message);
		// print_message("REPLY", reply);
		print_banner_message((kvs_header_t *)message);

		myssl_write(handler, reply, reply->totalsize);


		free(message);
		free(reply);
	}
	myssl_close(handler);

	// never come here
	myssl_free_handler(handler);
	return 0;
}
