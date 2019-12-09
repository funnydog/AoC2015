#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct symbol
{
	char *key;
	uint16_t value;
};

struct symtab
{
	struct symbol *table;
	size_t count;
	size_t size;
};

enum {
	ASSIGN,
	AND,
	OR,
	NOT,
	LSHIFT,
	RSHIFT,
};

struct rule
{
	int op;
	char *src1;
	char *src2;
	char *dst;
	int applied;
};

struct ruletable
{
	struct rule *table;
	size_t count;
	size_t size;
};

struct ruletable *ruletable_new(void)
{
	return calloc(1, sizeof(struct ruletable));
}

static void ruletable_free(struct ruletable *rt)
{
	if (rt) {
		while (rt->count-- > 0) {
			free(rt->table[rt->count].src1);
			free(rt->table[rt->count].src2);
			free(rt->table[rt->count].dst);
		}
		free(rt->table);
		free(rt);
	}
}

static struct ruletable *ruletable_load(FILE *input)
{
	struct ruletable *rt = ruletable_new();
	if (!rt)
		return NULL;

	char *line = NULL;
	size_t linesize = 0;
	while ((getline(&line, &linesize, input)) != -1) {
		struct rule r = {0};
		char *tok = strtok(line, " \n");
		int dst = 0;
		while (tok) {
			if (strcmp(tok, "AND") == 0) {
				r.op = AND;
			} else if (strcmp(tok, "OR") == 0) {
				r.op = OR;
			} else if (strcmp(tok, "NOT") == 0) {
				r.op = NOT;
			} else if (strcmp(tok, "LSHIFT") == 0) {
				r.op = LSHIFT;
			} else if (strcmp(tok, "RSHIFT") == 0) {
				r.op = RSHIFT;
			} else if (strcmp(tok, "->") == 0) {
				dst = 1;
			} else if (dst == 0 && r.src1 == NULL) {
				r.src1 = strdup(tok);
			} else if (dst == 0) {
				r.src2 = strdup(tok);
			} else if (dst) {
				r.dst = strdup(tok);
				break;
			}
			tok = strtok(NULL, " \n");
		}

		if (rt->count == rt->size) {
			size_t newsize = rt->size ? rt->size * 2 : 2;
			struct rule *newtable = realloc(rt->table, newsize * sizeof(newtable[0]));
			if (!newtable) {
				ruletable_free(rt);
				return NULL;
			}
			rt->size = newsize;
			rt->table = newtable;
		}

		rt->table[rt->count++] = r;
	}
	free(line);

	return rt;
}

static struct symtab *symtab_new(void)
{
	return calloc(1, sizeof(struct symtab));
}

static void symtab_free(struct symtab *st)
{
	if (st) {
		while (st->count-- > 0)
			free(st->table[st->count].key);

		free(st->table);
		free(st);
	}
}

static int symtab_find(struct symtab *sm, char *key, size_t *pos)
{
	if (sm->count == 0) {
		*pos = 0;
		return 0;
	}

	size_t low = 0;
	size_t high = sm->count-1;
	while (low < high) {
		size_t mid = low + (high - low) / 2;
		if (strcmp(sm->table[mid].key, key) < 0) {
			low = mid + 1;
		} else {
			high = mid;
		}
	}
	int c = strcmp(sm->table[high].key, key);
	if (c == 0) {
		*pos = high;
		return 1;
	} else if (c < 0) {
		*pos = high + 1;
		return 0;
	} else {
		*pos = high;
		return 0;
	}
}

static int symtab_add(struct symtab *sm, char *key, uint16_t value)
{
	size_t pos;
	if (symtab_find(sm, key, &pos))
		return -1;

	if (sm->count == sm->size) {
		size_t newsize = sm->size ? sm->size * 2 : 2;
		struct symbol *newtable = realloc(sm->table, newsize * sizeof(newtable[0]));
		if (!newtable)
			return -1;
		sm->size = newsize;
		sm->table = newtable;
	}

	memmove(sm->table + pos + 1, sm->table + pos, (sm->count - pos) * sizeof(sm->table[0]));
	sm->table[pos].key = strdup(key);
	sm->table[pos].value = value;
	sm->count++;
	return 0;
}

static int getvalue(struct symtab *st, char *str, uint16_t *value)
{
	size_t pos;
	if (symtab_find(st, str, &pos)) {
		*value = st->table[pos].value;
		return 1;
	}

	char *end;
	unsigned v = strtoul(str, &end, 10);
	if (str != end && end[0] == 0) {
		*value = v;
		return 1;
	}

	return 0;
}

static uint16_t apply_rules(struct symtab *st, struct ruletable *rt, char *endsym)
{
	size_t pos;
	while (!symtab_find(st, endsym, &pos)) {
		for (struct rule *r = rt->table; r < rt->table + rt->count; r++) {
			if (r->applied) {
				continue;
			} else if (r->op == ASSIGN) {
				uint16_t src1;
				if (getvalue(st, r->src1, &src1)) {
					symtab_add(st, r->dst, src1);
					r->applied = 1;
				}
			} else if (r->op == AND) {
				uint16_t src1, src2;
				if (getvalue(st, r->src1, &src1) &&
				    getvalue(st, r->src2, &src2)) {
					symtab_add(st, r->dst, src1 & src2);
					r->applied = 1;
				}
			} else if (r->op == OR) {
				uint16_t src1, src2;
				if (getvalue(st, r->src1, &src1) &&
				    getvalue(st, r->src2, &src2)) {
					symtab_add(st, r->dst, src1 | src2);
					r->applied = 1;
				}
			} else if (r->op == NOT) {
				uint16_t src1;
				if (getvalue(st, r->src1, &src1)) {
					symtab_add(st, r->dst, ~src1);
					r->applied = 1;
				}
			} else if (r->op == LSHIFT) {
				uint16_t src1, src2;
				if (getvalue(st, r->src1, &src1) &&
				    getvalue(st, r->src2, &src2)) {
					symtab_add(st, r->dst, src1 << src2);
					r->applied = 1;
				}
			} else if (r->op == RSHIFT) {
				uint16_t src1, src2;
				if (getvalue(st, r->src1, &src1) &&
				    getvalue(st, r->src2, &src2)) {
					symtab_add(st, r->dst, src1 >> src2);
					r->applied = 1;
				}
			} else {
				abort();
			}
		}
	}
	return st->table[pos].value;
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

	struct ruletable *rt = ruletable_load(input);
	fclose(input);
	if (!rt) {
		fprintf(stderr, "Cannot parse the signals\n");
		return -1;
	}

	uint16_t value;
	struct symtab *st = symtab_new();
	if (st) {
		value = apply_rules(st, rt, "a");
		printf("signal on \"a\": %u\n", value);
	}
	for (struct rule *r = rt->table; r < rt->table + rt->count; r++)
		r->applied = 0;

	symtab_free(st);
	st = symtab_new();
	if (st) {
		symtab_add(st, "b", value);
		printf("signal on \"a\": %u\n", apply_rules(st, rt, "a"));
	}

	symtab_free(st);
	ruletable_free(rt);
	return -1;
}
