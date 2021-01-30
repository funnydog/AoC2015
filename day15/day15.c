#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INGREDIENTS 16

enum
{
	CAPACITY,
	DURABILITY,
	FLAVOR,
	TEXTURE,
	CALORIES,
	MAX_PROPERTIES,
};

struct ingredient
{
	int properties[MAX_PROPERTIES];
};

struct recipe
{
	struct ingredient pool[MAX_INGREDIENTS];
	size_t count;
};

static void recipe_load(FILE *input, struct recipe *r)
{
	memset(r, 0, sizeof(*r));

	char *line = NULL;
	size_t linesize = 0;
	while (getline(&line, &linesize, input) != -1)
	{
		struct ingredient n;
		int i = 0;
		for (char *t = strtok(line, " \n,");
		     t;
		     t = strtok(NULL, " \n,"), i++)
		{
			char *end;
			if (i >= 2 && i <= 10 && (i&1) == 0)
			{
				n.properties[i / 2 - 1] = strtol(t, &end, 10);
				if (t == end || end[0] != 0)
				{
					break;
				}
			}
		}
		if (i < 10)
		{
			continue;
		}

		assert(r->count < MAX_INGREDIENTS);
		r->pool[r->count++] = n;
	}
	free(line);
}

static int recipe_score(const struct recipe *r, const int *a)
{
	int score = 1;
	for (int j = 0; j < CALORIES; j++)
	{
		int property = 0;
		for (size_t i = 0; i < r->count; i++)
		{
			property += r->pool[i].properties[j] * a[i];
		}
		if (property < 0)
		{
			property = 0;
		}

		score *= property;
	}
	return score;
}

static int recipe_calories(const struct recipe *r, const int *a)
{
	int calories = 0;
	for (size_t i = 0; i < r->count; i++)
	{
		calories += r->pool[i].properties[CALORIES] * a[i];
	}
	return calories;
}

static int next_sequence(int *a, size_t count, int *psum)
{
	size_t right = count-1;
	int sum = *psum;
	while (1)
	{
		right--;
		a[right]++;
		sum++;
		if (sum <= 100 || right == 0)
		{
			break;
		}
		sum-= a[right];
		a[right] = 0;
	}
	if (a[0] > 100)
	{
		return 0;
	}
	a[count - 1] = 100 - sum;
	*psum = sum;
	return 1;
}

static void solve(const struct recipe *r, int *ppart1, int *ppart2)
{
	int a[MAX_INGREDIENTS] = {0};
	int sum = 0;
	int part1 = INT_MIN;
	int part2 = INT_MIN;
	do
	{
		int score = recipe_score(r, a);
		if (part1 < score)
		{
			part1 = score;
		}
		if (recipe_calories(r, a) == 500
		    && part2 < score)
		{
			part2 = score;
		}
	} while (next_sequence(a, r->count, &sum));
	*ppart1 = part1;
	*ppart2 = part2;
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

	struct recipe r;
	recipe_load(input, &r);
	fclose(input);

	int part1, part2;
	solve(&r, &part1, &part2);
	printf("Part1: %d\n", part1);
	printf("Part2: %d\n", part2);
	return 0;
}
