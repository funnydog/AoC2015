#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
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
	struct aunt *pool;
	size_t count;
	size_t size;
};

static void aunts_free(struct aunts *a)
{
	if (a) {
		free(a->pool);
		free(a);
	}
}

static struct aunts *aunts_load(FILE *input)
{
	struct aunts *a = calloc(1, sizeof(*a));
	if  (!a)
		return NULL;

	char *line = NULL;
	size_t linesize = 0;
	while (getline(&line, &linesize, input) != -1) {
		struct aunt n = {0};
		for (size_t i = 0; i < COUNT; i++)
			n.prop[i] = -1;

		int type = 0;
		for (char *t = strtok(line, " ,:\n"); t; t = strtok(NULL, " ,:\n")) {
			int i;
			for (i = 0; i < COUNT; i++) {
				if (strcmp(types[i], t) == 0)
					break;
			}
			if (i < COUNT) {
				type = i;
				continue;
			}

			char *end;
			n.prop[type] = strtol(t, &end, 10);
			if (t == end || end[0] != 0)
				break;
		}

		if (a->count == a->size) {
			size_t newsize = a->size ? a->size * 2 : 2;
			struct aunt *newpool = realloc(a->pool, newsize * sizeof(newpool[0]));
			if (!newpool) {
				aunts_free(a);
				return NULL;
			}
			a->size = newsize;
			a->pool = newpool;
		}
		a->pool[a->count++] = n;
	}
	free(line);

	return a;
}

static void aunts_remove(struct aunts *a, size_t i)
{
	a->count--;
	memmove(a->pool+i, a->pool+i+1, (a->count - i) * sizeof(a->pool[0]));
}

static void remove_if_ne(struct aunts *a, int type, int value)
{
	for (size_t i = 0 ; i < a->count; ) {
		int prop = a->pool[i].prop[type];
		if (prop != -1 && prop != value) {
			aunts_remove(a, i);
		} else {
			i++;
		}
	}
}

static void remove_if_gte(struct aunts *a, int type, int value)
{
	for (size_t i = 0 ; i < a->count; ) {
		int prop = a->pool[i].prop[type];
		if (prop != -1 && prop >= value) {
			aunts_remove(a, i);
		} else {
			i++;
		}
	}
}

static void remove_if_lte(struct aunts *a, int type, int value)
{
	for (size_t i = 0 ; i < a->count; ) {
		int prop = a->pool[i].prop[type];
		if (prop != -1 && prop <= value) {
			aunts_remove(a, i);
		} else {
			i++;
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		return -1;
	}

	FILE *input = fopen(argv[1], "rb");
	if (!input) {
		fprintf(stderr, "Cannot open %s for reading\n", argv[1]);
		return -1;
	}

	struct aunts *a = aunts_load(input);
	fclose(input);

	if (!a) {
		fprintf(stderr, "Cannot parse the aunts\n");
		return -1;
	}

	remove_if_ne(a, CHILDREN, 3);
	remove_if_lte(a, CATS, 7);
	remove_if_ne(a, SAMOYEDS, 2);
	remove_if_gte(a, POMERANIANS, 3);
	remove_if_ne(a, AKITAS, 0);
	remove_if_ne(a, VIZSLAS, 0);
	remove_if_gte(a, GOLDFISH, 5);
	remove_if_lte(a, TREES, 3);
	remove_if_ne(a, CARS, 2);
	remove_if_ne(a, PERFUMES, 1);
	for (size_t i = 0; i < a->count; i++) {
		printf("Aunt: %d\n", a->pool[i].prop[ID]);
	}

	aunts_free(a);
	return 0;
}
