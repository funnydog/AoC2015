#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 512
#define NAME_SIZE 32
#define MAX_NAMES 64

struct helem
{
	char name[NAME_SIZE];
	size_t index;
};

struct table
{
	struct helem table[HASH_SIZE];
	size_t count;

	int adj[MAX_NAMES][MAX_NAMES];
	size_t sol[MAX_NAMES];
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

static size_t table_add(struct table *t, const char *name)
{
	assert(t->count < MAX_NAMES);
	size_t idx = hashfn(name) % HASH_SIZE;
	struct helem *n = t->table + idx;
	while (n->name[0] && strcmp(n->name, name))
	{
		if (++idx == HASH_SIZE)
		{
			idx = 0;
		}
		n = t->table + idx;
	}
	if (!n->name[0])
	{
		strcpy(n->name, name);
		n->index = t->count;
		t->count++;
	}
	return n->index;
}

static void table_load(FILE *input, struct table *t)
{
	memset(t, 0, sizeof(*t));

	char *line = NULL;
	size_t linesize = 0;
	while (getline(&line, &linesize, input) != -1)
	{
		size_t a = 0, b = 0;
		int happiness = 0;
		int i = 0;
		for (char *w = strtok(line, " \n.");
		     w;
		     w = strtok(NULL, " \n."), i++)
		{
			switch (i)
			{
			case 0:
				/* who gains or loses happiness */
				a = table_add(t, w);
				break;

			case 10:
				/* the attendee who is near */
				b = table_add(t, w);
				break;

			case 2:
				/* gains or loses? */
				happiness = strcmp(w, "gain") == 0 ? 1 : -1;
				break;

			case 3:
				/* amount of happiness */
				happiness *= strtol(w, NULL, 10);
				break;

			default:
				break;
			}
		}
		if (i > 10)
		{
			t->adj[a][b] = happiness;
		}
	}
	free(line);
}

static int table_happiness(const struct table *t)
{
	int sum = 0;
	assert(t->count >= 2);
	int l = t->sol[t->count-2];
	int i = t->sol[t->count-1];
	for (size_t ir = 0; ir < t->count; ir++)
	{
		sum += t->adj[i][l];
		sum += t->adj[i][t->sol[ir]];

		l = i;
		i = t->sol[ir];
	}
	return sum;
}

static int table_iterate(struct table *t, size_t l, size_t r)
{
	if (l == r)
	{
		return table_happiness(t);
	}
	else
	{
		int best = INT_MIN;
		for (size_t i = l; i <= r; i++)
		{
			size_t tmp = t->sol[l];
			t->sol[l] = t->sol[i];
			t->sol[i] = tmp;

			int cur = table_iterate(t, l+1, r);
			if (best < cur)
			{
				best = cur;
			}

			t->sol[i] = t->sol[l];
			t->sol[l] = tmp;
		}
		return best;
	}
}

static int table_max_happiness(struct table *t)
{
	for (size_t i = 0; i < t->count; i++)
	{
		t->sol[i] = i;
	}
	return table_iterate(t, 0, t->count-1);
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
		perror("Cannot open the file");
		return 1;
	}

	struct table t;
	table_load(input, &t);
	fclose(input);

	printf("Part1: %d\n", table_max_happiness(&t));

	table_add(&t, "me");
	printf("Part2: %d\n", table_max_happiness(&t));
	return 0;
}
