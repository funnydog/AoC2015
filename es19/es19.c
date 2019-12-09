#define _GNU_SOURCE
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct replacement
{
	char *from;
	char *to;
};

struct machine
{
	struct replacement *reps;
	size_t rcount;
	size_t rsize;

	char **molecules;
	size_t mcount;
	size_t msize;

	char *initial;
	size_t initlen;
	size_t min;
};

static int machine_find(struct machine *m, const char *s, size_t *pos)
{
	if (m->mcount == 0) {
		*pos = 0;
		return 0;
	}

	size_t low = 0;
	size_t high = m->mcount - 1;
	while (low < high) {
		size_t mid = low + (high - low) / 2;
		if (strcmp(m->molecules[mid], s) < 0) {
			low = mid + 1;
		} else {
			high = mid;
		}
	}

	int c = strcmp(m->molecules[high], s);
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

static int machine_add_molecule(struct machine *m, char *s)
{
	size_t pos;
	if (machine_find(m, s, &pos))
		return -1;

	if (m->mcount == m->msize) {
		size_t newsize = m->msize ? m->msize * 2 : 2;
		char **newm = realloc(m->molecules, newsize * sizeof(newm[0]));
		if (!newm)
			return -1;
		m->msize = newsize;
		m->molecules = newm;
	}

	memmove(m->molecules+pos+1, m->molecules+pos, (m->mcount - pos) * sizeof(m->molecules[0]));
	m->molecules[pos] = s;	/* previously allocated */
	m->mcount++;
	return 0;
}

static void replacement_enum(struct machine *m, struct replacement *r, const char *s)
{
	char *f = strstr(s, r->from);
	while (f) {
		char *res;
		asprintf(&res, "%.*s%s%s", (int)(f-s), s, r->to, f+strlen(r->from));
		if (machine_add_molecule(m, res) < 0)
			free(res);

		f = strstr(f + strlen(r->from), r->from);
	}
}

static void machine_free(struct machine *m)
{
	if (m) {
		while (m->mcount-- > 0)
			free(m->molecules[m->mcount]);

		while (m->rcount-- > 0) {
			free(m->reps[m->rcount].to);
			free(m->reps[m->rcount].from);
		}

		free(m->molecules);
		free(m->initial);
		free(m->reps);
		free(m);
	}
}

static int molecule_count(const char *mol, char *str)
{
	int count = 0;
	while ((mol = strstr(mol, str))) {
		mol += strlen(str);
		count++;
	}
	return count;
}

static int molecule_count_el(const char *mol)
{
	int count = 0;
	for (; *mol; mol++)
		if (isupper(*mol))
			count++;

	return count;
}

static struct machine *machine_load(FILE *input)
{
	struct machine *m = calloc(1, sizeof(*m));
	if (!m)
		return NULL;

	char *line = NULL;
	size_t linesize = 0;
	while (getline(&line, &linesize, input) != -1) {
		int i = 0;
		struct replacement r = {0};
		for (char *t = strtok(line, " \n"); t; t = strtok(NULL, " \n")) {
			if (i == 0) {
				r.from = strdup(t);
				if (!r.from) {
					machine_free(m);
					return NULL;
				}
				i++;
			} else if (i == 1) {
				/* skip */
				i++;
			} else if (i == 2) {
				r.to = strdup(t);
				if (!r.to) {
					machine_free(m);
					return NULL;
				}
				i++;
			}
		}
		if (i == 1) {
			free(m->initial);
			m->initial = r.from;
			m->initlen = strlen(r.from);
			continue;
		} else if (i != 3) {
			/* empty or malformed line */
			continue;
		}

		if (m->rcount == m->rsize) {
			size_t newsize = m->rsize ? m->rsize * 2 : 2;
			struct replacement *newreps = realloc(m->reps, newsize * sizeof(newreps[0]));
			if (!newreps) {
				machine_free(m);
				return NULL;
			}
			m->rsize = newsize;
			m->reps = newreps;
		}
		m->reps[m->rcount++] = r;
	}
	free(line);

	return m;
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

	struct machine *m = machine_load(input);
	fclose(input);

	if (!m) {
		fprintf(stderr, "Cannot parse the replacements\n");
		return -1;
	}

	for (size_t i = 0; i < m->rcount; i++)
		replacement_enum(m, m->reps + i, m->initial);

	printf("Distinct molecules: %zu\n", m->mcount);

	m->min = 100000;
	int tot = molecule_count_el(m->initial);
	int rn = molecule_count(m->initial, "Rn");
	int ar = molecule_count(m->initial, "Ar");
	int y = molecule_count(m->initial, "Y");
	printf("Fewest number of steps to go to from e to medicine: %d\n",
	       tot - (ar+rn) - y * 2 - 1);

	machine_free(m);
	return 0;
}
