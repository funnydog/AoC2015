#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct map
{
	char ***data;
	size_t width;
	size_t height;
	size_t size;
	int cur;
};

static void map_free(struct map *m)
{
	if (m) {
		while (m->height-- > 0) {
			free(m->data[1][m->height]);
			free(m->data[0][m->height]);
		}
		free(m->data[1]);
		free(m->data[0]);
		free(m->data);
		free(m);
	}
}

static struct map *map_load(FILE *input)
{
	struct map *m = calloc(1, sizeof(*m));
	if (!m)
		return NULL;

	/* allocate the back buffer */
	m->data = calloc(2, sizeof(m->data[0]));
	if (!m->data) {
		free(m);
		return NULL;
	}

	char *line = NULL;
	size_t linesize = 0;
	while (getline(&line, &linesize, input) != -1) {
		m->width = strlen(line)-1;
		/* chop the \n */
		line[m->width] = 0;
		if (m->height == m->size) {
			size_t newsize = m->height ? m->height * 2 : 2;
			char **data = realloc(m->data[0], newsize * sizeof(data[0]));
			if (!data) {
				map_free(m);
				return NULL;
			}
			m->size = newsize;
			m->data[0] = data;
			data = realloc(m->data[1], newsize * sizeof(data[0]));
			if (!data) {
				map_free(m);
				return NULL;
			}
			m->data[1] = data;
		}
		m->data[0][m->height] = strdup(line);
		m->data[1][m->height] = strdup(line);
		if (!m->data[0][m->height] || !m->data[1][m->height]) {
			m->height++;
			map_free(m);
			return NULL;
		}
		m->height++;
	}

	free(line);
	return m;
}

static char map_get(struct map *m, int x, int y)
{
	if (0 <= x && x < m->width && 0 <= y && y < m->height)
		return m->data[m->cur][y][x];
	return '.';
}

static void map_set(struct map *m, int x, int y, char v)
{
	if (0 <= x && x < m->width && 0 <= y && y < m->height)
		m->data[1-m->cur][y][x] = v;
}

static void map_swap(struct map *m)
{
	m->cur = 1 - m->cur;
}

static void map_cycle_common(struct map *m)
{
	const int off[][2] = {
		{ -1, -1 },	/* 1 */
		{  0, -1 },	/* 2 */
		{  1, -1 },	/* 3 */
		{  1,  0 },	/* 4 */
		{  1,  1 },	/* 5 */
		{  0,  1 },	/* 6 */
		{ -1,  1 },	/* 7 */
		{ -1,  0 },	/* 8 */
	};
	for (size_t y = 0; y < m->height; y++) {
		for (size_t x = 0; x < m->width; x++) {
			int neigh = 0;
			for (int i = 0; i < 8; i++) {
				if (map_get(m, x + off[i][0], y + off[i][1]) == '#')
					neigh++;
			}
			if (map_get(m, x, y) == '#') {
				map_set(m, x, y, (neigh == 2 || neigh == 3) ? '#' : '.');
			} else {
				map_set(m, x, y, (neigh == 3 ) ? '#' : '.');
			}
		}
	}
}
static void map_cycle(struct map *m)
{
	map_cycle_common(m);
	map_swap(m);
}

static void map_cycle_stuck(struct map *m)
{
	map_cycle_common(m);
	map_set(m, 0, 0, '#');
	map_set(m, m->width-1, 0, '#');
	map_set(m, m->width-1, m->height-1, '#');
	map_set(m, 0, m->height-1, '#');
	map_swap(m);
}

static size_t map_count_lights(struct map *m)
{
	size_t count = 0;
	for (size_t y = 0; y < m->height; y++) {
		for (size_t x = 0; x < m->width; x++) {
			if (m->data[m->cur][y][x] == '#')
				count++;
		}
	}
	return count;
}

static void map_print(struct map *m)
{
	for (size_t y = 0; y < m->height; y++) {
		printf("%s\n", m->data[m->cur][y]);
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

	struct map *m = map_load(input);
	if (!m) {
		fclose(input);
		fprintf(stderr, "Cannot parse the map\n");
		return -1;
	}

	for (int i = 0; i < 100; i++)
		map_cycle(m);

	printf("Lights on after 100 cycles: %zu\n", map_count_lights(m));
	map_free(m);

	rewind(input);
	m = map_load(input);
	fclose(input);
	if (!m) {
		fprintf(stderr, "Cannot parse the map\n");
		return -1;
	}

	for (int i = 0; i < 100; i++)
		map_cycle_stuck(m);

	printf("Lights on after 100 cycles: %zu\n", map_count_lights(m));
	map_free(m);
	return 0;
}
