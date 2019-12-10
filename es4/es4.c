#include <assert.h>
#include <stdio.h>
#include <openssl/md5.h>

int check_five_zeros(unsigned char *hash)
{
	return (hash[0] == 0 && hash[1] == 0 && (hash[2] & 0xf0) == 0);
}

int check_six_zeros(unsigned char *hash)
{
	return (hash[0] == 0 && hash[1] == 0 && (hash[2] & 0xff) == 0);
}

size_t find_num(const char *prefix, int (*fn)(unsigned char *))
{
	unsigned char result[MD5_DIGEST_LENGTH];
	char buffer[100];
	size_t i;
	for (i = 0; ; i++) {
		int len = snprintf(buffer, 100, "%s%zu", prefix, i);
		if (len > 0) {
			MD5((const unsigned char *)buffer, len, result);
		}
		if (fn(result))
		{
			break;
		}
	}
	return i;
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s start\n", argv[0]);
		return -1;
	}

	assert(find_num("abcdef", check_five_zeros) == 609043);
	assert(find_num("pqrstuv", check_five_zeros) == 1048970);

	printf("part1: %zu\n", find_num(argv[1], check_five_zeros));
	printf("part2: %zu\n", find_num(argv[1], check_six_zeros));
	return 0;
}
