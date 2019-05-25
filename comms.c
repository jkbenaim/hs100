#include <arpa/inet.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "comms.h"

bool hs100_encrypt(uint8_t *d, uint8_t *s, size_t len)
{
	uint8_t key, temp;
	size_t i;

	if (d == NULL)
		return false;
	if (s == NULL)
		return false;
	if (len == 0)
		return false;

	key = 0xab;
	for (i = 0; i < len; i++) {
		temp = key ^ s[i];
		key = temp;
		d[i] = temp;
	}
	return true;
}

bool hs100_decrypt(uint8_t *d, uint8_t *s, size_t len)
{
	uint8_t key, temp;
	size_t i;

	if (d == NULL)
		return false;
	if (s == NULL)
		return false;
	if (len == 0)
		return false;

	key = 0xab;
	for (i = 0; i < len; i++) {
		temp = key ^ s[i];
		key = s[i];
		d[i] = temp;
	}
	return true;
}

uint8_t *hs100_encode(size_t *outlen, char *srcmsg)
{
	size_t srcmsg_len;
	uint8_t *d;
	uint32_t temp;

	if (srcmsg == NULL)
		return NULL;

	srcmsg_len = strlen(srcmsg);
	*outlen = srcmsg_len + 4;
	d = calloc(1, *outlen);
	if (d == NULL)
		return NULL;
	if (!hs100_encrypt(d + 4, (uint8_t *) srcmsg, srcmsg_len)) {
		free(d);
		return NULL;
	}
	temp = htonl(srcmsg_len);
	memcpy(d, &temp, 4);

	return d;
}

char *hs100_decode(uint8_t *s, size_t s_len)
{
	uint32_t in_s_len;
	char *outbuf;

	if (s == NULL)
		return NULL;
	if (s_len <= 4)
		return NULL;

	memcpy(&in_s_len, s, 4);
	in_s_len = ntohl(in_s_len);
	if ((s_len - 4) < in_s_len) {
		/* packet was cut short- adjust in_s_len */
		in_s_len = s_len - 4;
	}

	outbuf = calloc(1, in_s_len + 1);

	if (!hs100_decrypt((uint8_t *) outbuf, s + 4, in_s_len)) {
		free(outbuf);
		return NULL;
	}

	return outbuf;
}

char *hs100_send(char *servaddr, char *msg)
{
	size_t s_len;
	int sock;
	uint8_t *s, *recvbuf;
	struct addrinfo hints, *res;
	uint32_t msglen;
	size_t recvsize;
	char *recvmsg = NULL;

	s = hs100_encode(&s_len, msg);
	if (s == NULL)
		return NULL;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_NUMERICSERV; // numeric port

	if (getaddrinfo(servaddr, "9999", &hints, &res) != 0)
		return NULL;

	sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sock < 0)
		goto out;

	if (connect(sock, res->ai_addr, res->ai_addrlen) < 0)
		goto out;

	send(sock, s, s_len, 0);
	free(s);
	recvsize = recv(sock, &msglen, sizeof(msglen), MSG_PEEK);
	if (recvsize != sizeof(msglen)) {
		goto out;
	}
	msglen = ntohl(msglen) + 4;
	recvbuf = calloc(1, (size_t) msglen);
	recvsize = recv(sock, recvbuf, msglen, MSG_WAITALL);
	close(sock);
	recvmsg = hs100_decode(recvbuf, msglen);
	free(recvbuf);

out:
	freeaddrinfo(res);
	return recvmsg;
}
