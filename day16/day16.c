#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum
{
	ID = 0,
	CHILDREN,
	CATS,
	SAMOYEDS,
	POMERANIANS,
	AKITAS,
	VIZSLAS,
	GOLDFISH,
	TREES,
	CARS,
	PERFUMES,
	COUNT,
	MAX_AUNTS = 500,
};

const char *types[] = {
	"Sue",
	"children",
	"cats",
	"samoyeds",
	"pomeranians",
	"akitas",
	"vizslas",
	"goldfish",
	"trees",
	"cars",
	"perfumes",
};

struct aunt
{
	int prop[COUNT];
};

struct aunts
{
	struct aunt pool[MAX_AUNTS];
	size_t count;
};

static struct aunts *aunts_load(FILE *input, struct aunts *a)
{
	memset(a, 0, sizeof(*a));

	char *line = NULL;
	size_t linesize = 0;
	while (getline(&line, &linesize, input) != -1)
	{
		struct aunt n;
		memset(&n, -1, sizeof(n));

		int type = 0;
		for (char *t = strtok(line, " ,:\n");
		     t;
		     t = strtok(NULL, " ,:\n"))
		{
			int i;
			for (i = 0; i < COUNT; i++)
			{
				if (strcmp(types[i], t) == 0)
				{
					break;
				}
			}
			if (i < COUNT)
			{
				type = i;
				continue;
			}

			char *end;
			n.prop[type] = strtol(t, &end, 10);
			if (t == end || end[0] != 0)
			{
				break;
			}
		}

		if (a->count < MAX_AUNTS)
		{
			a->pool[a->count++] = n;
		}
	}
	free(line);

	return a;
}

static void aunts_remove(struct aunts *a, size_t i)
{
	a->pool[i] = a->pool[--a->count];
}

static void remove_if_ne(struct aunts *a, int type, int value)
{
	size_t i = a->count;
	while (i-->0)
	{
		int prop = a->pool[i].prop[type];
		if (prop != -1 && prop != value)
		{
			aunts_remove(a, i);
		}
	}
}

static void remove_if_gte(struct aunts *a, int type, int value)
{
	size_t i = a->count;
	while (i-->0)
	{
		int prop = a->pool[i].prop[type];
		if (prop != -1 && prop >= value)
		{
			aunts_remove(a, i);
		}
	}
}

static void remove_if_lte(struct aunts *a, int type, int value)
{
	size_t i = a->count;
	while (i-->0)
	{
		int prop = a->pool[i].prop[type];
		if (prop != -1 && prop <= value)
		{
			aunts_remove(a, i);
		}
	}
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

	struct aunts a, b;
	aunts_load(input, &a);
	fclose(input);
	b = a;

	remove_if_ne(&a, CHILDREN, 3);
	remove_if_ne(&a, CATS, 7);
	remove_if_ne(&a, SAMOYEDS, 2);
	remove_if_ne(&a, POMERANIANS, 3);
	remove_if_ne(&a, AKITAS, 0);
	remove_if_ne(&a, VIZSLAS, 0);
	remove_if_ne(&a, GOLDFISH, 5);
	remove_if_ne(&a, TREES, 3);
	remove_if_ne(&a, CARS, 2);
	remove_if_ne(&a, PERFUMES, 1);
	if (a.count == 1)
	{
		printf("Part1: %d\n", a.pool[0].prop[ID]);
	}

	remove_if_ne(&b, CHILDREN, 3);
	remove_if_lte(&b, CATS, 7);
	remove_if_ne(&b, SAMOYEDS, 2);
	remove_if_gte(&b, POMERANIANS, 3);
	remove_if_ne(&b, AKITAS, 0);
	remove_if_ne(&b, VIZSLAS, 0);
	remove_if_gte(&b, GOLDFISH, 5);
	remove_if_lte(&b, TREES, 3);
	remove_if_ne(&b, CARS, 2);
	remove_if_ne(&b, PERFUMES, 1);
	if (b.count == 1)
	{
		printf("Part2: %d\n", b.pool[0].prop[ID]);
	}
	return 0;
}
