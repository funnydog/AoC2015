#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct agent
{
	int x, y;
};

struct map
{
	int xmin, ymin;
	int xmax, ymax;

	int start_x;
	int start_y;
	int width;
	int height;

	int *locations;
};

static void map_move_agent(struct map *m, struct agent *a, int c)
{
	switch (c) {
	case '<':
		a->x--;
		if (a->x < m->xmin)
			m->xmin = a->x;
		break;

	case '>':
		a->x++;
		if (a->x > m->xmax)
			m->xmax = a->x;
		break;

	case '^':
		a->y--;
		if (a->y < m->ymin)
			m->ymin = a->y;
		break;

	case 'v':
		a->y++;
		if (a->y > m->ymax)
			m->ymax = a->y;
		break;

	default:
		break;
	}
}

static int map_init(struct map *m, FILE *input)
{
	struct agent santa = {0};
	int c;
	memset(m, 0, sizeof(*m));
	while ((c = getc(input)) != EOF) {
		map_move_agent(m, &santa, c);
	}
	m->start_x = -m->xmin;
	m->start_y = -m->ymin;
	m->width = m->xmax + m->start_x + 1;
	m->height = m->ymax + m->start_y + 1;

	m->locations = malloc(m->width * m->height * sizeof(m->locations[0]));
	if (!m->locations)
		return -1;

	return 0;
}

static int map_robo_init(struct map *m, FILE *input)
{
	fseeko(input, 0, 0);
	struct agent santa = {0}, robo = {0};
	int i = 0;
	int c;
	memset(m, 0, sizeof(*m));
	while ((c = getc(input)) != EOF) {
		if (i & 1) {
			map_move_agent(m, &robo, c);
		} else {
			map_move_agent(m, &santa, c);
		}
		i++;
	}
	m->start_x = -m->xmin;
	m->start_y = -m->ymin;
	m->width = m->xmax + m->start_x + 1;
	m->height = m->ymax + m->start_y + 1;

	m->locations = malloc(m->width * m->height * sizeof(m->locations[0]));
	if (!m->locations)
		return -1;

	return 0;
}

static void map_destroy(struct map *m)
{
	free(m->locations);
}

static void map_visit(struct map *m, struct agent *a)
{
	m->locations[a->y * m->width + a->x]++;
}

static void map_populate(struct map *m, FILE *input)
{
	fseeko(input, 0, 0);
	memset(m->locations, 0, m->width * m->height * sizeof(m->locations[0]));
	int c;
	struct agent santa = {
		.x = m->start_x,
		.y = m->start_y,
	};
	map_visit(m, &santa);
	while ((c = getc(input)) != EOF) {
		map_move_agent(m, &santa, c);
		map_visit(m, &santa);
	}
}

static void map_robo_populate(struct map *m, FILE *input)
{
	fseeko(input, 0, 0);
	memset(m->locations, 0, m->width * m->height * sizeof(m->locations[0]));
	struct agent santa = {
		.x = m->start_x,
		.y = m->start_y,
	};
	map_visit(m, &santa);

	struct agent robo = {
		.x = m->start_x,
		.y = m->start_y,
	};
	map_visit(m, &robo);
	int i = 0, c;
	while ((c = getc(input)) != EOF) {
		if (i & 1) {
			map_move_agent(m, &robo, c);
			map_visit(m, &robo);
		} else {
			map_move_agent(m, &santa, c);
			map_visit(m, &santa);
		}
		i++;
	}
}

static int map_count(struct map *m)
{
	int count = 0;
	for (int i = 0; i < m->width * m->height; i++) {
		if (m->locations[i] > 0)
			count++;
	}
	return count;
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
		return -1;
	}

	FILE *input = fopen(argv[1], "rb");
	if (!input) {
		fprintf(stderr, "Cannot open %s for reading\n", argv[1]);
		return -1;
	}

	struct map m;
	map_init(&m, input);
	map_populate(&m, input);
	printf("Houses with at least one present: %d\n", map_count(&m));
	map_destroy(&m);

	map_robo_init(&m, input);
	map_robo_populate(&m, input);
	printf("Houses with at least one present (robo): %d\n", map_count(&m));
	map_destroy(&m);
	fclose(input);
	return 0;
}
