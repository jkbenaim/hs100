#include <stddef.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "comms.h"

#define RECV_BUF_SIZE	4096

bool hs100_encrypt(uint8_t *d, uint8_t *s, size_t len)
{
	if( d == NULL)
		return false;
	if (s == NULL)
		return false;
	if (len == 0)
		return false;

	uint8_t key = 0xab;
	for (size_t i = 0; i < len; i++) {
		uint8_t temp = key ^ s[i];
		key = temp;
		d[i] = temp;
	}
	return true;
}

bool hs100_decrypt(uint8_t *d, uint8_t *s, size_t len)
{
	if (d == NULL)
		return false;
	if (s == NULL)
		return false;
	if (len == 0)
		return false;

	uint8_t key = 0xab;
	for(size_t i=0; i<len; i++) {
		uint8_t temp = key ^ s[i];
		key = s[i];
		d[i] = temp;
	}
	return true;
}

uint8_t *hs100_encode(size_t *outlen, char *srcmsg)
{
	if (srcmsg == NULL)
		return NULL;

	size_t srcmsg_len = strlen(srcmsg);
	*outlen = srcmsg_len + 4;
	uint8_t *d = calloc(1, *outlen);
	if (d == NULL)
		return NULL;
	if (!hs100_encrypt(d + 4, (uint8_t *) srcmsg, srcmsg_len)) {
		free(d);
		return NULL;
	}
	uint32_t temp = htonl(srcmsg_len);
	memcpy(d, &temp, 4);

	return d;
}

char *hs100_decode(uint8_t *s, size_t s_len)
{
	if (s == NULL)
		return NULL;
	if (s_len <= 4)
		return NULL;

	uint32_t in_s_len;
	memcpy(&in_s_len, s, 4);
	in_s_len = ntohl(in_s_len);
	if ((s_len - 4) < in_s_len) {
		// packet was cut short- adjust in_s_len
		in_s_len = s_len - 4;
	}

	char *outbuf = calloc(1, in_s_len + 1);

	if (!hs100_decrypt((uint8_t*) outbuf, s + 4, in_s_len)) {
		free(outbuf);
		return NULL;
	}

	return outbuf;
}

char *hs100_send(char *servaddr, char *msg)
{
	size_t s_len;
	uint8_t *s = hs100_encode(&s_len, msg);
	if (s == NULL)
		return NULL;

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		return NULL;

	struct sockaddr_in address;
	memset(&address, '0', sizeof(struct sockaddr_in));

	address.sin_family = AF_INET;
	address.sin_port = htons(9999);

	if (inet_pton(AF_INET, servaddr, &address.sin_addr) <= 0)
		return NULL;

	if (connect(sock, (struct sockaddr *)&address,
		sizeof(struct sockaddr_in)) < 0)
		return NULL;

	send(sock, s, s_len, 0);
	free(s);
	uint8_t recvbuf[RECV_BUF_SIZE];
	size_t received_size = recv(sock, recvbuf, RECV_BUF_SIZE, 0);
	close(sock);
	if (received_size == 0)
		return NULL;
	char *recvmsg = hs100_decode(recvbuf, received_size);
	return recvmsg;
}
