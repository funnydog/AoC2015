#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CONTAINERS 20

struct containers
{
	size_t capacity[MAX_CONTAINERS];
	size_t count;
};

static void parse(FILE *input, struct containers *c)
{
	memset(c, 0, sizeof(*c));

	size_t val;
	while (c->count < MAX_CONTAINERS && fscanf(input, " %zu", &val) == 1)
	{
		c->capacity[c->count++] = val;
	}
}

static size_t table[MAX_CONTAINERS+1][MAX_CONTAINERS+1][150+1];

static size_t part1d(struct containers *c)
{
	memset(table, 0, sizeof(table));
	table[0][0][0] = 1;
	for (size_t j = 0; j < c->count; j++)
	{
		size_t cap = c->capacity[j];
		size_t s = 150+1;
		while (s-- > cap)
		{
			table[0][0][s] += table[0][0][s - cap];
		}
	}
	return table[0][0][150];
}

static size_t part2d(struct containers *c)
{
	memset(table, 0, sizeof(table));
	for (size_t i = 0; i <= c->count; i++)
	{
		table[0][i][0] = 1;
	}
	for (size_t count = 1; count <= c->count; count++)
	{
		for (size_t j = 1; j <= c->count; j++)
		{
			size_t cap = c->capacity[j-1];
			for (size_t s = 1; s <= 150; s++)
			{
				table[count][j][s] = table[count][j-1][s];
				if (s >= cap)
				{
					table[count][j][s] += table[count-1][j-1][s-cap];
				}
			}
		}
		if (table[count][c->count][150])
		{
			return table[count][c->count][150];
		}
	}

	return 0;
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
		perror(argv[1]);
		return 1;
	}

	struct containers c;
	parse(input, &c);
	fclose(input);

	printf("Part1: %zu\n", part1d(&c));
	printf("Part2: %zu\n", part2d(&c));
	return 0;
}
