#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "comms.h"

char *handler_associate(int argc, char *argv[])
{
	if (argc < 6) {
		fprintf(stderr, "not enough arguments\n");
		exit(1);
	}
	char *plug_addr = argv[1];
	char *ssid = argv[3];
	char *password = argv[4];
	char *key_type = argv[5];

	errno = 0;
	char *endptr;
	int key_type_num = (int)strtol(key_type, &endptr, 10);
	if (errno || endptr == key_type) {
		fprintf(stderr, "invalid key type: %s\n", key_type);
		exit(1);
	}

	const char *template =
		"{\"netif\":{\"set_stainfo\":{\"ssid\":\"%s\",\"password\":"
		"\"%s\",\"key_type\":%d}}}";

	size_t len = snprintf(NULL, 0, template, ssid, password,
				key_type_num);
	len++;	// snprintf does not count the null terminator

	char *msg = calloc(1, len);
	snprintf(msg, len, template, ssid, password, key_type_num);

	char *response = hs100_send(plug_addr, msg);

	return response;
}

char *handler_set_server(int argc, char *argv[])
{
	if (argc < 4) {
		fprintf(stderr, "not enough arguments\n");
		exit(1);
	}
	char *plug_addr = argv[1];
	char *server = argv[3];

	const char *template =
		"{\"cnCloud\":{\"set_server_url\":{\"server\":\"%s\"}}}";

	size_t len = snprintf(NULL, 0, template, server);
	len++;	// snprintf does not count the null terminator

	char *msg = calloc(1, len);
	snprintf(msg, len, template, server);

	char *response = hs100_send(plug_addr, msg);

	return response;
}
