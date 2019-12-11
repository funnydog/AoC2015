#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static uint64_t find_house(uint64_t points)
{
	size_t len = points / 10 + 1;
	uint64_t *house = calloc(len, sizeof(uint64_t));
	assert(house);
	for (size_t i = 1; i < len; i++)
	{
		for (size_t j = i; j < len; j += i)
		{
			house[j] += i * 10;
		}
	}
	for (size_t i = 1; i < len; i++)
	{
		if (house[i] >= points)
		{
			free(house);
			return i;
		}
	}
	free(house);
	return 0;
}

static uint64_t find_house2(uint64_t points)
{
	size_t len = points / 10 + 1;
	uint64_t *house = calloc(len, sizeof(uint64_t));
	assert(house);
	for (size_t i = 1; i < len; i++)
	{
		for (size_t j = 1; j < 51; j++)
		{
			if  (j * i < len)
			{
				house[j * i] += i * 11;
			}
		}
	}
	for (size_t i = 1; i < len; i++)
	{
		if (house[i] >= points)
		{
			free(house);
			return i;
		}
	}
	free(house);
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s num\n", argv[0]);
		return -1;
	}

	uint64_t points;
	if (sscanf(argv[1], "%" SCNu64, &points) != 1)
	{
		fprintf(stderr, "%s is not a valid value\n", argv[1]);
		return -1;
	}

	assert(find_house(10) == 1);
	assert(find_house(70) == 4);
	assert(find_house2(11) == 1);
	assert(find_house2(77) == 4);
	printf("part1: %" PRIu64 "\n", find_house(points));
	printf("part2: %" PRIu64 "\n", find_house2(points));
	return 0;
}
