#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* gate operators */
enum {
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
enum {
	UNKNOWN,
	DISCOVERED,
	PROCESSED,
};

/* type of edge */
enum {
	TREE,
	BACK,
	FORWARD,
	CROSS,
};

struct edge
{
	size_t y;
	struct edge *next;
};

struct vertex
{
	int state;
	size_t parent;
	size_t degree;
	size_t entry;
	size_t exit;
	struct edge *edges;

	struct gate data;
};

struct graph
{
	struct vertex *verts;
	size_t vcount;
	size_t vsize;
	size_t ecount;
	size_t time;

	size_t *stack;
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
	char *tok = strtok(line, " \n");
	while (tok) {
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
		tok = strtok(NULL, " \n");
	}
}

static uint16_t gate_eval(struct gate *g, struct symtab *st)
{
	switch (g->op) {
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

static void process_vertex_early(struct graph *g, size_t v)
{
}

static void process_vertex_late(struct graph *g, size_t v)
{
	g->stack[g->scount++] = v;
}

static int edge_type(struct graph *g, size_t x, size_t y)
{
	if (g->verts[y].parent == x) return TREE;
	if (g->verts[y].state == DISCOVERED) return BACK;
	if (g->verts[y].state == PROCESSED) {
		if (g->verts[y].entry > g->verts[x].exit) return FORWARD;
		if (g->verts[y].entry < g->verts[x].exit) return CROSS;
	}
	abort();
}
static void process_edge(struct graph *g, size_t x, size_t y)
{
	if (edge_type(g, x, y) == BACK) {
		printf("warning, found a back-edge, not a DAG\n");
	}
}

static void dfsr(struct graph *g, size_t v)
{
	g->verts[v].state = DISCOVERED;
	g->verts[v].entry = ++g->time;

	process_vertex_early(g, v);

	struct edge *p = g->verts[v].edges;
	while (p != NULL) {
		size_t y = p->y;
		if (g->verts[y].state == UNKNOWN) {
			g->verts[y].parent = v;
			process_edge(g, v, y);
			dfsr(g, y);
		} else {
			process_edge(g, v, y);
		}
		p = p->next;
	}

	process_vertex_late(g, v);
	g->verts[v].exit = ++g->time;
	g->verts[v].state = PROCESSED;
}

static void graph_free(struct graph *g)
{
	if (g) {
		while (g->vcount--) {
			free(g->verts[g->vcount].data.dst);
			free(g->verts[g->vcount].data.src2);
			free(g->verts[g->vcount].data.src1);

			struct edge *p = g->verts[g->vcount].edges;
			while (p != NULL) {
				struct edge *t = p;
				p = p->next;
				free(t);
			}
		}
		free(g->stack);
		free(g->verts);
		free(g);
	}
}

static int insert_edge(struct graph *g, size_t x, size_t y)
{
	struct edge *p = malloc(sizeof(*p));
	if (!p)
		return -1;

	p->y = y;
	p->next = g->verts[x].edges;
	g->verts[x].edges = p;
	g->verts[x].degree++;
	g->ecount++;
	return 0;
}

static int find_vertex(struct graph *g, const char *name, size_t *pos)
{
	if (g->vcount == 0 || name == NULL) {
		*pos = 0;
		return 0;
	}

	size_t low = 0;
	size_t high = g->vcount-1;
	while (low < high) {
		size_t mid = low + (high - low) / 2;
		if (strcmp(g->verts[mid].data.dst, name) < 0) {
			low = mid + 1;
		} else {
			high = mid;
		}
	}
	int c = strcmp(g->verts[high].data.dst, name);
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

static int vertex_cmp(const void *pa, const void *pb)
{
	const struct vertex *a = pa, *b = pb;
	return strcmp(a->data.dst, b->data.dst);
}

static struct graph *graph_load(FILE *input)
{
	struct graph *g = calloc(1, sizeof(*g));
	if (!g)
		return NULL;

	/* the vertices are the gates*/
	char *line = NULL;
	size_t linesize = 0;
	while (getline(&line, &linesize, input) != -1) {
		if (g->vcount == g->vsize) {
			size_t ns = g->vsize ? g->vsize * 2 : 2;
			struct vertex *nv = realloc(g->verts, ns * sizeof(nv[0]));
			if (!nv) {
				graph_free(g);
				return NULL;
			}
			g->vsize = ns;
			g->verts = nv;
		}
		g->verts[g->vcount].parent = SIZE_MAX;
		g->verts[g->vcount].degree = 0;
		g->verts[g->vcount].edges = NULL;
		gate_parse(line, &g->verts[g->vcount].data);
		g->vcount++;
	}
	free(line);

	/* sort the vertices by dst name for easier edge finding */
	qsort(g->verts, g->vcount, sizeof(g->verts[0]), vertex_cmp);

	/* now add the edges */
	for (size_t i = 0; i < g->vcount; i++) {
		size_t x;
		if (find_vertex(g, g->verts[i].data.src1, &x))
			insert_edge(g, x, i);
		if (find_vertex(g, g->verts[i].data.src2, &x))
			insert_edge(g, x, i);
	}

	/* allocate the stack of the topological sort */
	g->stack = malloc(g->vcount * sizeof(g->stack[0]));
	if (!g->stack) {
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
	for (size_t i = 0; i < g->vcount; i++) {
		g->verts[i].state = UNKNOWN;
		g->verts[i].parent = SIZE_MAX;
		g->verts[i].entry = g->verts[i].exit = 0;
	}
	g->scount = 0;
	g->time = 0;

	/* make a DFS for each connected component */
	for (size_t i = 0; i < g->vcount; i++) {
		if (g->verts[i].state == UNKNOWN)
			dfsr(g, i);
	}
}

/* evaluate the gates in topological order */
static void graph_eval(struct graph *g, struct symtab *st)
{
	size_t i = g->scount;
	while (i-- > 0) {
		size_t v = g->stack[i];
		struct gate *gate = &g->verts[v].data;
		symtab_add(st, gate->dst, gate_eval(gate, st));
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
	if (symtab_find(st, "a", &pos)) {
		uint16_t value = st->table[pos].value;
		printf("Value of the signal a: %u\n", value);
		printf("Overriding b with signal %u...\n", value);
		st->count = 0;
		symtab_add(st, "b", value);
		graph_eval(g, st);
		if (symtab_find(st, "a", &pos))
			printf("Value of the signal a: %u\n",
			       st->table[pos].value);
	}

	symtab_free(st);
	graph_free(g);
	return 0;
}
