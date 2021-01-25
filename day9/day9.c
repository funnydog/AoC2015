#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROUTES 32
#define NAME_SIZE 32
#define HASH_SIZE 256

struct helem
{
	char name[NAME_SIZE];
	size_t index;
};

static size_t hashfn(const char *key)
{
	size_t hash = 5381;
	while (*key)
	{
		hash = (hash * 33) ^ *key;
		key++;
	}
	return hash;
}

struct map
{
	struct helem table[HASH_SIZE];
	size_t count;

	int distance[MAX_ROUTES][MAX_ROUTES];
	int route[MAX_ROUTES];
};

static int map_add(struct map *m, const char *key, size_t *pos)
{
	size_t idx = hashfn(key) % HASH_SIZE;
	struct helem *h = m->table + idx;
	while (h->name[0] && strcmp(h->name, key))
	{
		if (++idx == HASH_SIZE)
		{
			idx = 0;
		}
		h = m->table + idx;
	}
	if (h->name[0])
	{
		*pos = h->index;
		return -1;
	}
	strcpy(h->name, key);
	*pos = h->index = m->count;
	m->count++;
	return 0;
}

static int map_load(FILE *input, struct map *m)
{
	memset(m, 0, sizeof(*m));
	for (size_t i = 0; i < MAX_ROUTES; i++)
	{
		for (size_t j = 0; j < MAX_ROUTES; j++)
		{
			m->distance[i][j] = INT_MAX;
		}
	}
	size_t sline = 0;
	char *line = NULL;
	while (getline(&line, &sline, input) != -1)
	{
		size_t i, j;
		int d = 0;
		int idx = 0;
		for (char *t = strtok(line, " ");
		     t;
		     t = strtok(NULL, " "), idx++)
		{
			switch (idx)
			{
			case 0:	map_add(m, t, &i); break;
			case 2:	map_add(m, t, &j); break;
			case 4:	d = strtol(t, NULL, 10); break;
			default: break;
			}
		}
		if (idx >= 4)
		{
			m->distance[i][j] = d;
			m->distance[j][i] = d;
		}
	}
	free(line);
	return 0;
}

static int map_route_length(struct map *m)
{
	int d = 0;
	for (size_t j = 1; j < m->count; j++)
	{
		d += m->distance[m->route[j-1]][m->route[j]];
	}
	return d;

}

static int shortest(struct map *m, size_t i, size_t last)
{
	if (i == last)
	{
		return map_route_length(m);
	}

	int best = INT_MAX;
	for (size_t j = i; j <= last; j++)
	{
		if (m->distance[m->route[i]][m->route[j]] == INT_MAX
		    && i != j)
		{
			continue;
		}

		/* move */
		int t = m->route[i];
		m->route[i] = m->route[j];
		m->route[j] = t;

		int sum = shortest(m, i+1, last);
		if (best > sum)
		{
			best = sum;
		}

		/* backtrack */
		m->route[j] = m->route[i];
		m->route[i] = t;
	}
	return best;
}

static int longest(struct map *m, size_t i, size_t last)
{
	if (i == last)
	{
		return map_route_length(m);
	}

	int best = 0;
	for (size_t j = i; j <= last; j++)
	{
		if (m->distance[m->route[i]][m->route[j]] == INT_MAX
		    && i != j)
		{
			continue;
		}

		/* move */
		int t = m->route[i];
		m->route[i] = m->route[j];
		m->route[j] = t;

		int sum = longest(m, i+1, last);
		if (best < sum)
		{
			best = sum;
		}

		/* backtrack */
		m->route[j] = m->route[i];
		m->route[i] = t;
	}
	return best;
}

static int map_best_route(struct map *m, int(*fn)(struct map *m, size_t, size_t))
{
	for (size_t i = 0; i < m->count; i++)
	{
		m->route[i] = i;
	}
	return fn(m, 0, m->count-1);
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

	struct map m;
	int r = map_load(input, &m);
	fclose(input);
	if (r < 0)
	{
		fprintf(stderr, "Cannot parse the data\n");
		return 1;
	}

	printf("Part1: %d\n", map_best_route(&m, shortest));
	printf("Part2: %d\n", map_best_route(&m, longest));
	return 0;
}
