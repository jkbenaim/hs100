#include <string.h>
#include <stdlib.h>
#include "escape.h"

char *escape_json(const char *orig)
{
	char *out = NULL;
	size_t len = 0;
	char *outp;

	out = malloc(strlen((const char *)orig) * 2 + 1);
	if (!out) {
		return NULL;
	}

	outp = out;

	while (*orig) switch (*orig) {
	case '\\':
		*outp++ = '\\';
		*outp++ = '\\';
		orig++;
		len += 2;
		break;
	case '\"':
		*outp++ = '\\';
		*outp++ = '\"';
		orig++;
		len += 2;
		break;
	default:
		*outp++ = *orig++;
		len++;
		break;
	}

	out = realloc(out, len);
	return out;
}
