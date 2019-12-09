#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
	CAPACITY = 0,
	DURABILITY,
	FLAVOR,
	TEXTURE,
	CALORIES,
	TOTAL,
};

struct ingredient
{
	char *name;
	int properties[5];
};

struct recipe
{
	struct ingredient *pool;
	size_t count;
	size_t size;
};

static void recipe_free(struct recipe *r)
{
	if (r) {
		while (r->count-- > 0)
			free(r->pool[r->count].name);
		free(r->pool);
		free(r);
	}
}

static struct recipe *recipe_load(FILE *input)
{
	struct recipe *r = calloc(1, sizeof(r[0]));
	if (!r)
		return NULL;

	char *line = NULL;
	size_t linesize = 0;
	while (getline(&line, &linesize, input) != -1) {
		struct ingredient n;
		int i = 0;
		for (char *t = strtok(line, " \n,"); t; t = strtok(NULL, " \n,"), i++) {
			char *end;
			if (i == 0) {
				n.name = strdup(t);
				if (!n.name)
					break;
			} else if (i >= 2 && i <= 10 && (i&1) == 0) {
				n.properties[i / 2 - 1] = strtol(t, &end, 10);
				if (t == end || end[0] != 0) {
					free(n.name);
					break;
				}
			}
		}
		if (i < 10) {
			free(n.name);
			continue;
		}

		if (r->count == r->size) {
			size_t newsize = r->size ? r->size * 2 : 2;
			struct ingredient *newpool = realloc(r->pool, newsize * sizeof(newpool[0]));
			if (!newpool) {
				recipe_free(r);
				return NULL;
			}
			r->size = newsize;
			r->pool = newpool;
		}
		r->pool[r->count++] = n;
	}
	free(line);
	return r;
}

static int recipe_score(struct recipe *r, int *a)
{
	int score = 1;
	for (int j = 0; j < CALORIES; j++) {
		int property = 0;
		for (size_t i = 0; i < r->count; i++)
			property += r->pool[i].properties[j] * a[i];
		if (property < 0)
			property = 0;

		score *= property;
	}
	return score;
}

static int recipe_calories(struct recipe *r, int *a)
{
	int calories = 0;
	for (size_t i = 0; i < r->count; i++) {
		calories += r->pool[i].properties[CALORIES] * a[i];
	}
	return calories;
}

static int next_sequence(int *a, size_t count)
{
	int sum;
	size_t p = count-1;
	while (p-- > 0) {
		a[p]++;
		sum = 0;
		for (size_t i = 0; i < count-1; i++) {
			sum += a[i];
		}
		if (sum <= 100)
			break;

		a[p] = 0;
	}
	a[count - 1] = 100 - sum;
	return sum <= 100;
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

	struct recipe *r = recipe_load(input);
	fclose(input);

	if (!r) {
		fprintf(stderr, "Cannot parse the recipe\n");
		return -1;
	}

	int *a = calloc(r->count, sizeof(a[0]));
	if (a) {
		int max = INT_MIN;
		int cal;
		do {
			int score = recipe_score(r, a);
			if (max < score) {
				max = score;
				cal = recipe_calories(r, a);
			}
		} while (next_sequence(a, r->count));
		printf("Max score with %d calories: %d\n", cal, max);

		memset(a, 0, r->count * sizeof(a[0]));
		max = INT_MIN;
		do {
			int score = recipe_score(r, a);
			if (max < score && recipe_calories(r, a) == 500)
				max = score;
		} while (next_sequence(a, r->count));
		free(a);

		printf("Max score with 500 calories: %d\n", max);
	}


	recipe_free(r);
	return 0;
}
