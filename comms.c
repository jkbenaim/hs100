#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif /* _WIN32 */

#include "comms.h"

static bool hs100_encrypt(uint8_t *d, const uint8_t *s, size_t len)
{
	uint8_t key;
	size_t i;

	if (d == NULL)
		return false;
	if (s == NULL)
		return false;
	if (len == 0)
		return false;

	key = 0xab;
	for (i = 0; i < len; i++) {
		uint8_t temp = key ^ s[i];
		key = temp;
		d[i] = temp;
	}
	return true;
}

static bool hs100_decrypt(uint8_t *d, const uint8_t *s, size_t len)
{
	uint8_t key;
	size_t i;

	if (d == NULL)
		return false;
	if (s == NULL)
		return false;
	if (len == 0)
		return false;

	key = 0xab;
	for (i = 0; i < len; i++) {
		uint8_t temp = key ^ s[i];
		key = s[i];
		d[i] = temp;
	}
	return true;
}

static uint8_t *hs100_encode(size_t *outlen, const char *srcmsg)
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
	if (!hs100_encrypt(d + 4, (const uint8_t *) srcmsg, srcmsg_len)) {
		free(d);
		return NULL;
	}
	temp = htonl(srcmsg_len);
	memcpy(d, &temp, 4);

	return d;
}

static char *hs100_decode(const uint8_t *s, size_t s_len)
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

char *hs100_send(const char *servaddr, const char *msg)
{
	size_t s_len;
	struct sockaddr_in address;
	char *recvmsg;
	uint32_t msglen;
#ifdef _WIN32
	char *s, *recvbuf;
	int recvsize;
	unsigned int sock;
	WSADATA d;

	s = (char *)hs100_encode(&s_len, msg);
#else
	uint8_t *s, *recvbuf;
	int sock;
	size_t recvsize;

	s = hs100_encode(&s_len, msg);
#endif /* _WIN32 */

	if (s == NULL)
		return NULL;

#ifdef _WIN32
	if (WSAStartup(MAKEWORD(2, 2), &d))
		return NULL;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		return NULL;
#else
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		return NULL;
#endif /* _WIN32 */

	memset(&address, '0', sizeof(struct sockaddr_in));

	address.sin_family = AF_INET;
	address.sin_port = htons(9999);

	if (inet_pton(AF_INET, servaddr, &address.sin_addr) <= 0) {
                struct hostent *lh = gethostbyname(servaddr);

                if (lh == NULL) {
		        return NULL;
                }

                if (lh->h_addrtype != AF_INET) {
		        return NULL;
                }

                if (lh->h_addr_list[0] == NULL) {
		        return NULL;
                }

                memcpy(&address.sin_addr, lh->h_addr_list[0], lh->h_length);
        }

	if (connect(sock, (const struct sockaddr*)&address, sizeof(struct sockaddr_in)) < 0)
	{
		return NULL;
	}

	send(sock, s, s_len, 0);
	free(s);
	recvsize = recv(sock, (char *)&msglen, sizeof(msglen), MSG_PEEK);
	if (recvsize != sizeof(msglen)) {
		return NULL;
	}
	msglen = ntohl(msglen) + 4;
	recvbuf = calloc(1, (size_t) msglen);
	(void) recv(sock, recvbuf, msglen, MSG_WAITALL);
#ifdef _WIN32
	closesocket(sock);
#else
	close(sock);
#endif /* _WIN32 */
	recvmsg = hs100_decode((const uint8_t *)recvbuf, msglen);
	free(recvbuf);
	return recvmsg;
}
