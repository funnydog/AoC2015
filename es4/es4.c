#include <stdio.h>
#include <openssl/md5.h>

int check_zeros(unsigned char *hash)
{
	return (hash[0] == 0 && hash[1] == 0 && (hash[2] & 0xFf) == 0);
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s start\n", argv[0]);
		return -1;
	}

	unsigned char result[MD5_DIGEST_LENGTH];
	char buffer[100];

	size_t i;
	for (i = 0; ; i++) {
		int len = snprintf(buffer, 100, "%s%zu", argv[1], i);
		if (len > 0) {
			MD5((const unsigned char *)buffer, len, result);
		}
		if (check_zeros(result))
			break;
	}
	printf("found %zu\n", i);
	return 0;
}
