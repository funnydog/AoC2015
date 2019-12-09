#include <stdio.h>
#include <stdlib.h>

struct containers
{
	int *p;
	size_t count;
	size_t size;
};

struct comb
{
	size_t *a;
	size_t elements;
	size_t maxcount;
};

static struct comb *comb_new(size_t maxcount, size_t elements)
{
	if (maxcount < elements)
		return NULL;

	struct comb *c = malloc(sizeof(*c) + elements * sizeof(c->a[0]));
	if (c) {
		c->a = (void*)((char *)c + sizeof(*c));
		c->elements = elements;
		c->maxcount = maxcount;
		for (size_t i = 0; i < elements; i++)
			c->a[i] = i;
	}
	return c;
}

static void comb_free(struct comb *c)
{
	free(c);
}

static int comb_next(struct comb *c)
{
	size_t p = c->elements;
	size_t max = c->maxcount;
	while (p-- >0) {
		c->a[p]++;
		if (c->a[p] < max) {
			while (++p < c->elements)
				c->a[p] = c->a[p-1] + 1;

			return 1;
		}
		max--;
	}
	return 0;
}

static void containers_free(struct containers *c)
{
	if (c) {
		free(c->p);
		free(c);
	}
}

static struct containers *containers_load(FILE *input)
{
	struct containers *c = calloc(1, sizeof(c[0]));
	if (!c)
		return NULL;

	int val;
	while (fscanf(input, " %d", &val) == 1) {
		if (c->count == c->size) {
			size_t newsize = c->size ? c->size * 2 : 2;
			int *newp = realloc(c->p, newsize * sizeof(newp[0]));
			if (!newp) {
				containers_free(c);
				return NULL;
			}
			c->size = newsize;
			c->p = newp;
		}
		c->p[c->count++] = val;
	}

	return c;
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, "Usage: %s <filename> <litres>\n", argv[0]);
		return -1;
	}

	char *end;
	int litres = strtol(argv[2], &end, 10);
	if (argv[2] == end || end[0] != 0) {
		fprintf(stderr, "Cannot parse the litres %s\n", argv[2]);
		return -1;
	}

	FILE *input = fopen(argv[1], "rb");
	if (!input) {
		fprintf(stderr, "Cannot open %s for reading\n", argv[1]);
		return -1;
	}

	struct containers *co = containers_load(input);
	fclose(input);
	if (!co) {
		fprintf(stderr, "Cannot parse the container collection\n");
		return -1;
	}

	size_t count = 0;
	size_t min = co->count;
	for (size_t i = 1; i < co->count; i++) {
		struct comb *cm = comb_new(co->count, i);
		do {
			int sum = 0;
			for (size_t j = 0; j < i; j++)
				sum += co->p[cm->a[j]];

			if (sum == litres) {
				count++;
				if (min > i)
					min = i;
			}
		} while (comb_next(cm));
		comb_free(cm);
	}
	printf("Number of the possible combinations: %zu\n", count);
	printf("Minimum containers that fit %d litres: %zu\n", litres, min);

	count = 0;
	struct comb *cm = comb_new(co->count, min);
	do {
		int sum = 0;
		for (size_t j = 0; j < min; j++)
			sum += co->p[cm->a[j]];

		if (sum == litres)
			count++;
	} while (comb_next(cm));
	comb_free(cm);
	containers_free(co);
	printf("Number of possible combination of %zu containers: %zu\n", min, count);

	return 0;
}
