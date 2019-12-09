#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct map
{
	char **table;
	size_t count;
	size_t size;

	int **adj;
};

struct permutations
{
	int *num;
	int *dir;
	size_t count;
};

static struct permutations *permutations_new(size_t count)
{
	struct permutations *p = malloc(sizeof(*p) + count * sizeof(int) + count * sizeof(int));
	if (p) {
		p->num = (void *)((char *)p + sizeof(*p));
		p->dir = (void *)((char *)p + sizeof(*p) + count * sizeof(int));
		p->count = count;
		for (size_t i = 0; i < count; i++) {
			p->num[i] = i;
			p->dir[i] = 0;
		}
	}
	return p;
}

static void permutations_free(struct permutations *p)
{
	free(p);
}

static int permutations_find_largest(struct permutations *p, size_t *pos)
{
	int vmax = INT_MIN;
	size_t imax = p->count;
	for (size_t i = 0; i < p->count; i++) {
		int other;
		if (!p->dir[i] && i > 0) {
			/* looking left */
			other = i-1;
		} else if (p->dir[i] && i < p->count - 1) {
			/* looking right */
			other = i+1;
		} else {
			continue;
		}
		if (p->num[i] > p->num[other] && vmax < p->num[i]) {
			vmax = p->num[i];
			imax = i;
		}
	}
	if (vmax == INT_MIN)
		return 0;

	*pos = imax;
	return 1;
}

static int permutations_next(struct permutations *p)
{
	size_t pos;
	if (!permutations_find_largest(p, &pos))
		return 0;

	size_t other = p->dir[pos] ? pos + 1 : pos - 1;
	int k = p->num[pos];
	p->num[pos] = p->num[other];
	p->num[other] = k;
	int d = p->dir[pos];
	p->dir[pos] = p->dir[other];
	p->dir[other] = d;

	for (size_t i = 0; i < p->count; i++)
		if (p->num[i] > k)
			p->dir[i] = 1 - p->dir[i];

	return 1;
}

static void map_free(struct map *m)
{
	if (m) {
		while (m->count-- > 0) {
			free(m->table[m->count]);
			if (m->adj) free(m->adj[m->count]);
		}
		free(m->adj);
		free(m->table);
		free(m);
	}
}

static int map_find(struct map *m, const char *name, size_t *pos)
{
	if (m->count == 0) {
		*pos = 0;
		return 0;
	}

	size_t low = 0;
	size_t high = m->count;
	while (low < high) {
		size_t mid = low + (high - low) / 2;
		if (strcmp(m->table[mid], name) < 0) {
			low = mid + 1;
		} else {
			high = mid;
		}
	}
	int c = strcmp(m->table[high], name);
	if (c < 0) {
		*pos = high + 1;
		return 0;
	} else if (c > 0) {
		*pos = high;
		return 0;
	} else {
		*pos = high;
		return 1;
	}
}

static int map_add(struct map *m, const char *name)
{
	size_t pos;
	if (map_find(m, name, &pos))
		return -1;

	if (m->count == m->size) {
		size_t newsize = m->size ? m->size * 2 : 2;
		char **newtable = realloc(m->table, newsize * sizeof(newtable[0]));
		if (!newtable)
			return -1;
		m->table = newtable;
		m->size = newsize;
	}

	memmove(m->table + pos + 1, m->table + pos, (m->count - pos) * sizeof(m->table[0]));
	m->table[pos] = strdup(name);
	m->count++;
	return 0;
}

static void map_set_distance(struct map *m, const char *a, const char *b, int d)
{
	size_t pa, pb;
	if (!map_find(m, a, &pa) || !map_find(m, b, &pb))
		return;

	m->adj[pa][pb] = m->adj[pb][pa] = d;
}

static struct map *map_load(FILE *input)
{
	struct map *m = calloc(1, sizeof(*m));
	if (!m)
		return NULL;

	char *line = NULL;
	size_t linesize = 0;
	while (getline(&line, &linesize, input) != -1) {
		int i = 0;
		for (char *t = strtok(line, " \n"); t != NULL; t = strtok(NULL, " \n")) {
			if (i == 0) {
				if (strcmp(t, "to") == 0) {
					i++;
				} else {
					map_add(m, t);
				}
			} else if (i == 1) {
				if (strcmp(t, "=") == 0) {
					i++;
				} else {
					map_add(m, t);
					break;
				}
			}
		}
	}
	m->adj = malloc(m->count * sizeof(m->adj[0]));
	if (!m->adj) {
		map_free(m);
		return NULL;
	}
	for (size_t i = 0; i < m->count; i++) {
		m->adj[i] = malloc(m->count * sizeof(m->adj[0][0]));
		if (!m->adj[i]) {
			map_free(m);
			return NULL;
		}
		for (size_t j = 0; j < m->count; j++) {
			m->adj[i][j] = INT_MAX;
		}
	}
	rewind(input);
	while (getline(&line, &linesize, input) != -1) {
		int i = 0;
		char *l1, *l2;
		for (char *t = strtok(line, " \n"); t != NULL; t = strtok(NULL, " \n")) {
			if (i == 0) {
				if (strcmp(t, "to") == 0) {
					i++;
				} else {
					l1 = t;
				}
			} else if (i == 1) {
				if (strcmp(t, "=") == 0) {
					i++;
				} else {
					l2 = t;
				}
			} else if (i == 2) {
				char *end;
				int dist = strtol(t, &end, 10);
				if (t != end && end[0] == 0) {
					map_set_distance(m, l1, l2, dist);
					break;
				}
			}
		}
	}
	free(line);
	return m;
}

static int map_distance(struct map *m, int *pts)
{
	int dst = 0;
	for (size_t i = 0; i < m->count-1; i++) {
		dst += m->adj[pts[i]][pts[i+1]];
	}
	return dst;
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
	fclose(input);
	if (!m) {
		fprintf(stderr, "Cannot parse the map\n");
		return -1;
	}

	int mindist = INT_MAX;
	int maxdist = INT_MIN;
	struct permutations *p = permutations_new(m->count);
	do {
		int d = map_distance(m, p->num);
		if (mindist > d)
			mindist = d;
		if (maxdist < d)
			maxdist = d;
	} while (permutations_next(p));
	permutations_free(p);

	printf("Shortest distance: %d\n", mindist);
	printf("Longest distance: %d\n", maxdist);
	map_free(m);
	return 0;
}
