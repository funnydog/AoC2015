#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 32771

struct pos
{
	int x, y;
};

static int pos_move(struct pos *p, char d)
{
	switch (d)
	{
	case '^': p->y--; break;
	case '>': p->x++; break;
	case 'v': p->y++; break;
	case '<': p->x--; break;
	default: return 0;
	}
	return 1;
}

static size_t hashpos(struct pos *p)
{
	return (p->x << 8) ^ p->y;
}

struct hash
{
	struct pos table[HASH_SIZE];
	size_t count;
};

static void hash_init(struct hash *h)
{
	for (size_t i = 0; i < HASH_SIZE; i++)
	{
		h->table[i].x = INT_MAX;
	}
	h->count = 0;
}

static void hash_add(struct hash *h, struct pos *p)
{
	size_t idx = hashpos(p) % HASH_SIZE;
	struct pos *n = h->table + idx;
	while  (n->x != INT_MAX && memcmp(n, p, sizeof(*p)))
	{
		if  (++idx == HASH_SIZE)
		{
			idx = 0;
		}
		n = h->table + idx;
	}
	if (n->x == INT_MAX)
	{
		*n = *p;
		h->count++;
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
		return 1;
	}

	FILE *input = fopen(argv[1], "rb");
	if (!input)
	{
		fprintf(stderr, "Cannot open %s\n", argv[1]);
		return 1;
	}

	size_t sline = 0;
	char *line = NULL;
	int r = getline(&line, &sline, input);
	fclose(input);
	if (r == -1)
	{
		fputs("Cannot read the data\n", stderr);
		return 1;
	}

	struct hash map;
	hash_init(&map);
	struct pos santa = {0, 0};
	hash_add(&map, &santa);
	for (size_t i = 0; ; i++)
	{
		if (!pos_move(&santa, line[i]))
		{
			break;
		}
		hash_add(&map, &santa);
	}
	printf("Part1: %zu\n", map.count);

	hash_init(&map);
	santa.x = santa.y = 0;
	struct pos robosanta = {0, 0};
	hash_add(&map, &santa);
	for (size_t i = 0; ; i++)
	{
		struct pos *p = i%2 ? &robosanta : &santa;
		if (!pos_move(p, line[i]))
		{
			break;
		}
		hash_add(&map, p);
	}
	printf("Part2: %zu\n", map.count);
	free(line);
	return 0;
}
