#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REINDEER 32

struct reindeer
{
	int speed;
	int flight_time;
	int rest_time;
};

static int reindeer_distance(const struct reindeer *r, int t)
{
	div_t a = div(t, r->flight_time + r->rest_time);
	if (a.rem > r->flight_time)
	{
		a.rem = r->flight_time;
	}
	return r->speed * (a.quot * r->flight_time + a.rem);
}

struct game
{
	struct reindeer reind[MAX_REINDEER];
	size_t count;
};

static void game_load(FILE *input, struct game *g)
{
	memset(g, 0, sizeof(*g));
	char *line = NULL;
	size_t linesize = 0;
	while (getline(&line, &linesize, input) != -1)
	{
		struct reindeer r = {0};
		int i = 0;
		for (char *t = strtok(line, " \n");
		     t;
		     t = strtok(NULL, " \n"), i++)
		{
			switch (i)
			{
			case 3:
				r.speed = strtol(t, NULL, 10);
				break;

			case 6:
				r.flight_time = strtol(t, NULL, 10);
				break;

			case 13:
				r.rest_time = strtol(t, NULL, 10);
				break;

			default:
				break;
			}
		}
		if (i < 13)
		{
			continue;
		}
		g->reind[g->count++] = r;
	}
 	free(line);
}

static int winner_distance(const struct game *g, int seconds)
{
	int max = INT_MIN;
	for (size_t i = 0; i < g->count; i++)
	{
		int d = reindeer_distance(g->reind+i, seconds);
		if (max < d)
		{
			max = d;
		}
	}
	return max;
}

static int winner_points(const struct game *g, int seconds)
{
	int points[MAX_REINDEER] = {0};
	int dist[MAX_REINDEER] = {0};
	int max;
	for (int t = 1; t <= seconds; t++)
	{
		max = INT_MIN;
		for (size_t i = 0; i < g->count; i++)
		{
			dist[i] = reindeer_distance(g->reind+i, t);
			if (max < dist[i])
			{
				max = dist[i];
			}
		}
		for (size_t i = 0; i < g->count; i++)
		{
			if (dist[i] == max)
			{
				points[i]++;
			}
		}
	}

	max = INT_MIN;
	for (size_t i = 0; i < g->count; i++)
	{
		if (max < points[i])
		{
			max = points[i];
		}
	}
	return max;
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

	struct game g;
	game_load(input, &g);
	fclose(input);

	printf("Part1: %d\n", winner_distance(&g, 2503));
	printf("Part2: %d\n", winner_points(&g, 2503));
	return 0;
}
