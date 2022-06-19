#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comms.h"
#include "escape.h"

char *handler_associate(int argc, char *argv[])
{
	const char *template =
		"{\"netif\":{\"set_stainfo\":{\"ssid\":\"%s\",\"password\":"
		"\"%s\",\"key_type\":%d}}}";
	char *plug_addr = argv[1];
	char *ssid = argv[3];
	char *password = argv[4];
	char *key_type = argv[5];
	char *endptr, *msg, *response;
	int key_type_num;
	size_t len;

	if (password)
		password = escape_json(password);

	if (argc < 6) {
		fprintf(stderr, "not enough arguments\n");
		free(password);
		exit(1);
	}
	errno = 0;
	key_type_num = (int)strtol(key_type, &endptr, 10);
	if (errno || endptr == key_type) {
		fprintf(stderr, "invalid key type: %s\n", key_type);
		free(password);
		exit(1);
	}

	len = snprintf(NULL, 0, template, ssid, password,
			key_type_num);
	len++;	/* snprintf does not count the null terminator */

	msg = calloc(1, len);
	snprintf(msg, len, template, ssid, password, key_type_num);

	response = hs100_send(plug_addr, msg);
	free(password);
	return response;
}

char *handler_set_server(int argc, char *argv[])
{
	const char *template =
		"{\"cnCloud\":{\"set_server_url\":{\"server\":\"%s\"}}}";
	char *plug_addr = argv[1];
	char *server = argv[3];
	size_t len;
	char *msg, *response;

	if (argc < 4) {
		fprintf(stderr, "not enough arguments\n");
		exit(1);
	}
	len = snprintf(NULL, 0, template, server);
	len++;	/* snprintf does not count the null terminator */

	msg = calloc(1, len);
	snprintf(msg, len, template, server);

	response = hs100_send(plug_addr, msg);

	return response;
}
