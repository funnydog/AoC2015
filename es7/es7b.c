#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* gate operators */
enum
{
	ASSIGN,
	AND,
	OR,
	NOT,
	LSHIFT,
	RSHIFT,
};

struct gate
{
	int op;
	char *src1;
	char *src2;
	char *dst;
};

struct symbol
{
	const char *key;
	uint16_t value;
};

struct symtab
{
	struct symbol *table;
	size_t count;
	size_t size;
};

/* state of the vertex */
enum
{
	UNKNOWN,
	DISCOVERED,
	PROCESSED,
};

/* type of edge */
enum
{
	TREE,
	BACK,
	FORWARD,
	CROSS,
};

struct edge
{
	struct vertex *target;
	struct edge *next;
};

struct vertex
{
	int state;
	struct vertex *parent;
	size_t degree;
	size_t entry;
	size_t exit;
	struct edge *edges;
	struct gate data;

	struct vertex *next;
};

#define TABLE_SIZE 1024

struct graph
{
	struct vertex *table[TABLE_SIZE];
	size_t vcount;
	size_t ecount;
	size_t time;

	struct vertex **stack;
	size_t scount;
};

static struct symtab *symtab_new(void)
{
	return calloc(1, sizeof(struct symtab));
}

static void symtab_free(struct symtab *st)
{
	if (st) {
		free(st->table);
		free(st);
	}
}

static int symtab_find(struct symtab *sm, const char *key, size_t *pos)
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

static int symtab_add(struct symtab *sm, const char *key, uint16_t value)
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
	sm->table[pos].key = key;
	sm->table[pos].value = value;
	sm->count++;
	return 0;
}

static uint16_t symtab_get_value(struct symtab *st, const char *str)
{
	size_t pos;
	if (symtab_find(st, str, &pos))
		return st->table[pos].value;

	char *end;
	unsigned v = strtoul(str, &end, 10);
	if (str != end && end[0] == 0)
		return v;

	abort();
}

static void gate_parse(char *line, struct gate *g)
{
	memset(g, 0, sizeof(*g));
	/* g->op = ASSIGN; */
	int dst = 0;
	for (char *tok = strtok(line, " \n");
	     tok;
	     tok = strtok(NULL, " \n"))
	{
		if (strcmp(tok, "AND") == 0) {
			g->op = AND;
		} else if (strcmp(tok, "OR") == 0) {
			g->op = OR;
		} else if (strcmp(tok, "NOT") == 0) {
			g->op = NOT;
		} else if (strcmp(tok, "LSHIFT") == 0) {
			g->op = LSHIFT;
		} else if (strcmp(tok, "RSHIFT") == 0) {
			g->op = RSHIFT;
		} else if (strcmp(tok, "->") == 0) {
			dst = 1;
		} else if (dst == 0 && g->src1 == NULL) {
			g->src1 = strdup(tok);
		} else if (dst == 0) {
			g->src2 = strdup(tok);
		} else if (dst) {
			g->dst = strdup(tok);
			break;
		}
	}
}

static uint16_t gate_eval(struct gate *g, struct symtab *st)
{
	switch (g->op)
	{
	case ASSIGN:
		return symtab_get_value(st, g->src1);
	case AND:
		return symtab_get_value(st, g->src1) & symtab_get_value(st, g->src2);
	case OR:
		return symtab_get_value(st, g->src1) | symtab_get_value(st, g->src2);
	case NOT:
		return ~symtab_get_value(st, g->src1);
	case LSHIFT:
		return symtab_get_value(st, g->src1) << symtab_get_value(st, g->src2);
	case RSHIFT:
		return symtab_get_value(st, g->src1) >> symtab_get_value(st, g->src2);
	default:
		abort();
	}
}

static void process_vertex_early(struct graph *g, struct vertex *v)
{
}

static void process_vertex_late(struct graph *g, struct vertex *v)
{
	g->stack[g->scount++] = v;
}

static int edge_type(struct graph *g, struct vertex *x, struct vertex *y)
{
	if (y->parent == x)
	{
		return TREE;
	}

	if (y->state == DISCOVERED)
	{
		return BACK;
	}
	if (y->state == PROCESSED)
	{
		if (y->entry > x->entry)
		{
			return FORWARD;
		}

		if (y->entry < x->entry)
		{
			return CROSS;
		}
	}
	abort();
}

static void process_edge(struct graph *g, struct vertex *x, struct vertex *y)
{
	if (edge_type(g, x, y) == BACK)
	{
		fprintf(stderr, "found a back-edge, not a DAG\n");
		abort();
	}
}

static void dfsr(struct graph *g, struct vertex *v)
{
	v->state = DISCOVERED;
	v->entry = ++g->time;

	process_vertex_early(g, v);
	for (struct edge *p = v->edges; p; p = p->next)
	{
		struct vertex *y = p->target;
		if (y->state == UNKNOWN)
		{
			y->parent = v;
			process_edge(g, v, y);
			dfsr(g, y);
		}
		else
		{
			process_edge(g, v, y);
		}
	}

	process_vertex_late(g, v);
	v->exit = ++g->time;
	v->state = PROCESSED;
}

static void graph_free(struct graph *g)
{
	if (g)
	{
		for (int i = 0; i < TABLE_SIZE; i++)
		{
			struct vertex *v = g->table[i];
			while (v)
			{
				struct vertex *tv = v;
				v = v->next;

				struct edge *e = tv->edges;
				while (e)
				{
					struct edge *te = e;
					e = e->next;
					free(te);
				}
				free(tv->data.dst);
				free(tv->data.src2);
				free(tv->data.src1);
				free(tv);
			}
		}
		free(g->stack);
		free(g);
	}
}

static int insert_edge(struct graph *g, struct vertex *x, struct vertex *y)
{
	struct edge *p = malloc(sizeof(*p));
	if (!p)
	{
		return -1;
	}

	p->target = y;
	p->next = x->edges;
	x->edges = p;
	x->degree++;
	g->ecount++;
	return 0;
}

static unsigned hashfn(const char *word)
{
	unsigned hash = 5381;
	while (*word)
	{
		hash = hash * 33 + *word++;
	}
	return hash;
}

static struct vertex *find_vertex(struct graph *g, const char *name)
{
	unsigned pos = hashfn(name) & (TABLE_SIZE-1);
	struct vertex *v = g->table[pos];
	while (v && strcmp(v->data.dst, name) != 0)
	{
		v = v->next;
	}
	return v;
}

static struct vertex *add_vertex(struct graph *g, const char *name)
{
	unsigned pos = hashfn(name) & (TABLE_SIZE-1);
	struct vertex *v = g->table[pos];
	while (v && strcmp(v->data.dst, name) != 0)
	{
		v = v->next;
	}
	if (!v)
	{
		v = calloc(1, sizeof(*v));
		v->next = g->table[pos];
		g->table[pos] = v;
		g->vcount++;
	}
	return v;
}

static struct graph *graph_load(FILE *input)
{
	struct graph *g = calloc(1, sizeof(*g));
	if (!g)
	{
		return NULL;
	}

	/* the vertices are the gates*/
	char *line = NULL;
	size_t lsize = 0;
	while (getline(&line, &lsize, input) != -1)
	{
		struct gate data = {};
		gate_parse(line, &data);

		struct vertex *v = add_vertex(g, data.dst);
		if (v)
		{
			v->data = data;
		}
	}
	free(line);

	/* now add the edges */
	for (size_t i = 0; i < TABLE_SIZE; i++)
	{
		for (struct vertex *v = g->table[i];
		     v;
		     v = v->next)
		{
			struct vertex *src;
			if (v->data.src1 && (src = find_vertex(g, v->data.src1)))
			{
				insert_edge(g, src, v);
			}

			if (v->data.src2 && (src = find_vertex(g, v->data.src2)))
			{
				insert_edge(g, src, v);
			}
		}
	}

	/* allocate the stack of the topological sort */
	g->stack = malloc(g->vcount * sizeof(g->stack[0]));
	if (!g->stack)
	{
		graph_free(g);
		return NULL;
	}
	g->scount = 0;

	return g;
}

/* topological sort */
static void graph_build_deps(struct graph *g)
{
	/* initialize the DFS */
	for (size_t i = 0; i < TABLE_SIZE; i++)
	{
		for (struct vertex *v = g->table[i];
		     v;
		     v = v->next)
		{
			v->state = UNKNOWN;
			v->parent = NULL;
			v->entry = v->exit = 0;
		}
	}
	g->scount = 0;
	g->time = 0;

	/* make a DFS for each connected component */
	for (size_t i = 0; i < TABLE_SIZE; i++)
	{
		for (struct vertex *v = g->table[i];
		     v;
		     v = v->next)
		{
			if (v->state == UNKNOWN)
			{
				dfsr(g, v);
			}
		}
	}
}

/* evaluate the gates in topological order */
static void graph_eval(struct graph *g, struct symtab *st)
{
	size_t i = g->scount;
	while (i-- > 0)
	{
		struct vertex *v = g->stack[i];
		symtab_add(st, v->data.dst, gate_eval(&v->data, st));
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

	struct graph *g = graph_load(input);
	fclose(input);
	if (!g) {
		fprintf(stderr, "Cannot load the circuit\n");
		return -1;
	}
	graph_build_deps(g);

	struct symtab *st = symtab_new();
	if (!st) {
		fprintf(stderr, "Cannot create the symtable\n");
		graph_free(g);
		return -1;
	}
	graph_eval(g, st);

	size_t pos;
	if (symtab_find(st, "a", &pos))
	{
		uint16_t value = st->table[pos].value;
		printf("part1: %u\n", value);
		/* overriding b with signal a */
		st->count = 0;
		symtab_add(st, "b", value);
		graph_eval(g, st);
		if (symtab_find(st, "a", &pos))
		{
			printf("part2: %u\n", st->table[pos].value);
		}
	}

	symtab_free(st);
	graph_free(g);
	return 0;
}
