#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct problem
{
	int *weights;
	size_t wcount;
	size_t total;
};

static void problem_free(struct problem *p)
{
	if (p)
	{
		free(p->weights);
		free(p);
	}
}

static struct problem *problem_read(FILE *input)
{
	struct problem *p = calloc(1, sizeof(*p));
	if (!p)
	{
		return NULL;
	}
	size_t wsize = 0;
	int t;
	while (fscanf(input, " %d", &t) == 1)
	{
		if (p->wcount == wsize)
		{
			size_t nsize = wsize ? wsize * 2 : 8;
			int *nw = realloc(p->weights, nsize * sizeof(*nw));
			assert(nw);
			wsize = nsize;
			p->weights = nw;
		}
		p->weights[p->wcount++] = t;
		p->total += t;
	}
	return p;
}

static size_t search(struct problem *p, int groups)
{
	size_t c[100];
	assert(p->wcount < 100);

	size_t target = p->total / groups;
	size_t bent = SIZE_MAX;

	for (size_t n = 1; n <= p->wcount; n++)
	{
		for (size_t i = 0; i < n; i++)
		{
			c[i] = i;
		}

		for (;;)
		{
			/* compute the weight and the entanglement */
			size_t w = 0;
			size_t e = 1;
			for (size_t i = 0; i < n; i++)
			{
				w += p->weights[c[i]];
				e *= p->weights[c[i]];
			}
			if (w == target && e < bent)
			{
				bent = e;
			}

			/* get the next combination */
			size_t place = n;
			size_t max = p->wcount;
			while (place-- > 0)
			{
				c[place]++;
				if (c[place] < max)
				{
					while (++place < n)
					{
						c[place] = c[place-1]+1;
					}
					break;
				}
				max--;
			}
			if (place == SIZE_MAX)
			{
				break;
			}
		}

		/* exit if we found the best entanglement */
		if (bent != SIZE_MAX)
		{
			break;
		}
	}
	return bent;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <input>\n", argv[0]);
		return -1;
	}

	FILE *input = fopen(argv[1], "rb");
	if (!input)
	{
		fprintf(stderr, "Cannot open %s for reading\n", argv[1]);
		return -1;
	}

	struct problem *p = problem_read(input);
	fclose(input);
	if (!p)
	{
		fprintf(stderr, "Cannt load the values\n");
		return -1;
	}

	printf("part1: %zu\n", search(p, 3));
	printf("part2: %zu\n", search(p, 4));

	problem_free(p);
	return 0;
}
