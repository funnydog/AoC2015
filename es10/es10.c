#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t append(char **dst, size_t *dstlen, size_t ilen, int repeats, int cur)
{
	for (;;) {
		size_t avail = *dstlen - ilen;
		int slen = snprintf(*dst + ilen, avail, "%d%c", repeats, cur);
		if (slen <0) {
			abort();
		} else if ((size_t)slen >= avail) {
			size_t newsize = ilen + slen + 1;
			char *newdst = realloc(*dst, newsize);
			if (!newdst)
				abort();

			*dstlen = newsize;
			*dst = newdst;
		} else {
			return slen + ilen;
		}
	}
}

static void encode(const char *src, char **dst, size_t *dstlen)
{
	int repeats = 1;
	int cur = *src++;
	size_t ilen = 0;
	for (; src[0]; src++) {
		if (src[0] == cur) {
			repeats++;
			continue;
		} else {
			ilen = append(dst, dstlen, ilen, repeats, cur);
			cur = src[0];
			repeats = 1;
		}
	}
	append(dst, dstlen, ilen, repeats, cur);
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <startingdigits>\n", argv[0]);
		return -1;
	}

	char *src = strdup(argv[1]);
	for (int i = 0; i < 50; i++)
	{
		char *dst = NULL;
		size_t dstlen = 0;
		encode(src, &dst, &dstlen);
		free(src);
		src = dst;
		if (i + 1 == 40)
		{
			printf("part1: %zu\n", strlen(src));
		}
		if (i + 1 == 50)
		{
			printf("part2: %zu\n", strlen(src));
		}
	}
	free(src);

	return 0;
}
