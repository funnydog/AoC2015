#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct reindeer
{
	char *name;
	int speed;
	int flight_time;
	int rest_time;

	int points;
};

static int reindeer_distance(struct reindeer *r, int t)
{
	int cycles = t / (r->flight_time + r->rest_time);
	int mod = t % (r->flight_time + r->rest_time);

	int distance = cycles * r->speed * r->flight_time;
	if (mod < r->flight_time)
		distance += r->speed * mod;
	else
		distance += r->speed * r->flight_time;

	return distance;
}

struct game
{
	struct reindeer *pool;
	size_t count;
	size_t size;
};

static void game_free(struct game *g)
{
	if (g) {
		while (g->count-- > 0) {
			free(g->pool[g->count].name);
		}
		free(g->pool);
		free(g);
	}
}

static struct game *game_load(FILE *input)
{
	struct game *g = calloc(1, sizeof(g[0]));
	if (!g)
		return NULL;

	char *line = NULL;
	size_t linesize = 0;
	while (getline(&line, &linesize, input) != -1) {
		struct reindeer r = {0};
		int i = 0;
		for (char *t = strtok(line, " \n"); t; t = strtok(NULL, " \n"), i++) {
			char *end;
			switch (i) {
			case 0:
				r.name = strdup(t);
				if (!r.name)
					continue;
				break;
			case 3:
				r.speed = strtol(t, &end, 10);
				if (t == end || end[0] != 0) {
					free(r.name);
					continue;
				}
				break;

			case 6:
				r.flight_time = strtol(t, &end, 10);
				if  (t == end || end[0] != 0) {
					free(r.name);
					continue;
				}
				break;

			case 13:
				r.rest_time = strtol(t, &end, 10);
				if (t == end || end[0] != 0) {
					free(r.name);
					continue;
				}
				break;

			default:
				break;
			}
		}
		if (i < 13) {
			free(r.name);
			continue;
		}

		if (g->count == g->size) {
			size_t newsize = g->size ? g->size * 2 : 2;
			struct reindeer *newpool = realloc(g->pool, newsize * sizeof(newpool[0]));
			if (!newpool) {
				game_free(g);
				return NULL;
			}
			g->size = newsize;
			g->pool = newpool;
		}
		g->pool[g->count++] = r;

	}
 	free(line);
	return g;
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, "Usage: %s <filename> <seconds>\n", argv[0]);
		return -1;
	}

	char *end;
	int seconds = strtoul(argv[2], &end, 10);
	if (argv[2] == end || end[0] != 0) {
		fprintf(stderr, "Cannot parse the seconds %s\n", argv[2]);
		return -1;
	}

	FILE *input = fopen(argv[1], "rb");
	if (!input) {
		fprintf(stderr, "Cannot open %s for reading\n", argv[1]);
		return -1;
	}

	struct game *g = game_load(input);
	fclose(input);

	if (!g) {
		fprintf(stderr, "Cannot parse the game\n");
		return -1;
	}

	printf("%10s | distance\n-----------+---------\n", "reindeer");
	int max = INT_MIN;
	for (size_t i = 0; i < g->count; i++) {
		int d = reindeer_distance(g->pool+i, seconds);
		printf("%10s | %d\n", g->pool[i].name, d);
		if (max < d) {
			max = d;
		}
	}
	printf("Distance traveled by the winning reindeer: %d\n", max);

	for (int t = 1; t <= seconds; t++) {
		max = INT_MIN;
		for (size_t i = 0; i < g->count; i++) {
			int d = reindeer_distance(g->pool+i, t);
			if (max <= d) {
				max = d;
			}
		}
		if (max == INT_MIN)
			continue;

		for (size_t i = 0; i < g->count; i++) {
			if (max == reindeer_distance(g->pool+i, t))
				g->pool[i].points++;
		}
	}

	max = INT_MIN;
	for (size_t i = 0; i < g->count; i++) {
		printf("%10s: %d\n", g->pool[i].name, g->pool[i].points);
		if (max < g->pool[i].points)
			max = g->pool[i].points;
	}
	printf("Points earned by the winning reindeer: %d\n", max);

	game_free(g);
	return 0;
}
