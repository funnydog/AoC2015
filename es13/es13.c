#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct perm
{
	int *num;
	size_t *dir;
	size_t count;
	size_t i;
};

static void perm_reset(struct perm *p)
{
	for (size_t i = 0; i < p->count; i++) {
		p->num[i] = i;
		p->dir[i] = 0;
	}
	p->i = 0;
}

static struct perm *perm_new(size_t count)
{
	struct perm *p = malloc(sizeof(p[0]) +
				count * sizeof(p->num[0]) +
				count * sizeof(p->dir[0]));
	if (p) {
		p->num = (void *)((char *)p + sizeof(p[0]));
		p->dir = (void *)((char *)p + sizeof(p[0]) + count * sizeof(p->num[0]));
		p->count = count;
		perm_reset(p);
	}
	return p;
}

static void perm_free(struct perm *p)
{
	free(p);
}

static int perm_next(struct perm *p)
{
	/* heap's algorithm */
	size_t i = p->i;
	while (i < p->count) {
		if (p->dir[i] < i) {
			if (i & 1) {
				int t = p->num[p->dir[i]];
				p->num[p->dir[i]] = p->num[i];
				p->num[i] = t;
			} else {
				int t = p->num[0];
				p->num[0] = p->num[i];
				p->num[i] = t;
			}
			p->dir[i]++;
			i = 0;
			break;
		} else {
			p->dir[i] = 0;
			i++;
		}
	}

	p->i = i;
	return i < p->count;
}

struct table
{
	char **attendees;
	size_t count;
	size_t size;

	int **adj;
};

static int table_find(struct table *t, const char *name, size_t *pos)
{
	if (t->count == 0) {
		*pos = 0;
		return 0;
	}

	size_t low = 0;
	size_t high = t->count-1;
	while (low < high) {
		size_t mid = low + (high - low) / 2;
		if (strcmp(t->attendees[mid], name) < 0) {
			low = mid + 1;
		} else {
			high = mid;
		}
	}
	int c = strcmp(t->attendees[high], name);
	if (c < 0) {
		*pos = high+1;
		return 0;
	} else if (c > 0) {
		*pos = high;
		return 0;
	} else {
		*pos = high;
		return 1;
	}
}

static int table_add_attendee(struct table *t, const char *name)
{
	size_t pos;
	if (table_find(t, name, &pos))
		return -1;

	if (t->count == t->size) {
		size_t newsize = t->size ? t->size * 2 : 2;
		char **newatt = realloc(t->attendees, newsize * sizeof(newatt[0]));
		if (!newatt)
			return -1;

		t->size = newsize;
		t->attendees = newatt;
	}

	memmove(t->attendees + pos + 1, t->attendees + pos,
		(t->count - pos) * sizeof(t->attendees[0]));
	t->attendees[pos] = strdup(name);
	t->count++;
	return 0;
}

static void table_free(struct table *t)
{
	if (t) {
		while (t->count-- > 0) {
			free(t->attendees[t->count]);
			if (t->adj)
				free(t->adj[t->count]);
		}
		free(t->attendees);
		free(t->adj);
		free(t);
	}
}

static struct table *table_load(FILE *input)
{
	struct table *t = calloc(1, sizeof(t[0]));
	if (!t)
		return NULL;

	/* first pass create the attendees */
	char *line = NULL;
	size_t linesize = 0;
	while (getline(&line, &linesize, input) != -1) {
		int i = 0;
		for (char *w = strtok(line, " \n."); w; w = strtok(NULL, " \n."), i++) {
			if (i == 0) {
				table_add_attendee(t, w);
			} else if (i == 10) {
				table_add_attendee(t, w);
			}
		}
	}

	/* second pass create the adjacency matrix */
	t->adj = malloc(t->count * sizeof(t->adj[0]));
	if (!t->adj)
		abort();
	for (size_t i = 0; i < t->count; i++) {
		t->adj[i] = calloc(t->count, sizeof(t->adj[0][0]));
		if (!t->adj[i])
			abort();
	}
	rewind(input);
	while (getline(&line, &linesize, input) != -1) {
		int i = 0, happiness;
		size_t a1, a2;
		for (char *w = strtok(line, " \n."); w; w = strtok(NULL, " \n."), i++) {
			if (i == 0) {
				if (!table_find(t, w, &a1))
					abort();
			} else if (i == 10) {
				if (!table_find(t, w, &a2))
					abort();
			} else if (i == 2) {
				if (strcmp(w, "lose") == 0)
					happiness = -1;
				else
					happiness = 1;
			} else if (i == 3) {
				char *end;
				happiness *= strtol(w, &end, 10);
				if (w == end)
					abort();
			}
		}
		t->adj[a1][a2] = happiness;
	}
	free(line);

	return t;
}

static int table_happiness(struct table *t, int *pos)
{
	int sum = 0;
	for (size_t i = 0; i < t->count; i++) {
		size_t left = (i + t->count - 1) % t->count;
		size_t right = (i + 1) % t->count;
		sum += t->adj[pos[i]][pos[left]];
		sum += t->adj[pos[i]][pos[right]];
	}
	return sum;
}

static int table_max_happiness(struct table *t)
{
	struct perm *p = perm_new(t->count);
	if (!p)
		return -1;

	int max = INT_MIN;
	do {
		int happiness = table_happiness(t, p->num);
		if (max < happiness) {
			max = happiness;
		}
	} while (perm_next(p));
	perm_free(p);
	return max;
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

	struct table *t = table_load(input);
	fclose(input);

	if (!t) {
		fprintf(stderr, "Cannot parse the table\n");
		return -1;
	}

	printf("Maximum happiness without me: %d\n", table_max_happiness(t));

	/* hack to avoid to reload the adjacency matrix */
	table_add_attendee(t, "zzzzzzzzzme");
	t->adj = realloc(t->adj, t->count * sizeof(t->adj[0]));
	if (!t->adj)
		abort();
	t->adj[t->count-1] = NULL;
	for (size_t i = 0; i < t->count; i++) {
		t->adj[i] = realloc(t->adj[i], t->count * sizeof(t->adj[0][0]));
		if (!t->adj[i])
			abort();
		t->adj[i][t->count-1] = 0;
	}
	for (size_t i = 0; i < t->count; i++)
		t->adj[t->count-1][i] = 0;

	printf("Maximum happiness with me: %d\n", table_max_happiness(t));

	table_free(t);
	return 0;
}
