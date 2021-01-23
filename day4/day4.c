#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/md5.h>

static int check_five_zeros(unsigned char *hash)
{
	return (hash[0]|hash[1]|(hash[2] & 0xf0)) == 0;
}

static int check_six_zeros(unsigned char *hash)
{
	return (hash[0]|hash[1]|hash[2]) == 0;
}

static size_t find_num(const char *prefix, int (*fn)(unsigned char *))
{
	unsigned char result[MD5_DIGEST_LENGTH];
	MD5_CTX c;
	char buffer[100];
	size_t i;
	for (i = 0; ; i++)
	{
		int len = snprintf(buffer, sizeof(buffer), "%s%zu", prefix, i);
		if (len > 0)
		{
			MD5_Init(&c);
			MD5_Update(&c, buffer, len);
			MD5_Final(result, &c);
			if (fn(result))
			{
				break;
			}
		}
	}
	return i;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		return 1;
	}

	FILE *input = fopen(argv[1], "rb");
	if (!input)
	{
		fprintf(stderr, "Cannot open %s\n", argv[1]);
		return 1;
	}

	size_t ssalt = 0;
	char *salt = NULL;
	int r = getline(&salt, &ssalt, input);
	fclose(input);
	if (r == -1)
	{
		fputs("Cannot read the data\n", stderr);
		return 1;
	}

	ssalt = strlen(salt);
	if (salt[ssalt-1] == '\n')
	{
		salt[ssalt-1] = 0;
	}

	printf("part1: %zu\n", find_num(salt, check_five_zeros));
	printf("part2: %zu\n", find_num(salt, check_six_zeros));
	free(salt);
	return 0;
}
