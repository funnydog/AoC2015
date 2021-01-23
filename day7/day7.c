#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	char name[16];
	int  op;
	char src1[16];
	char src2[16];
	uint16_t value;
	int computed;
};

#define HASH_SIZE 4096

struct circuit
{
	struct gate table[HASH_SIZE];
	size_t count;
};

static unsigned hashfn(const char *key)
{
	unsigned hash = 5381;
	while (*key)
	{
		hash = (hash * 33) ^ *key;
		key++;
	}
	return hash;
}

static int circuit_find(struct circuit *c, const char *key, size_t *pos)
{
	size_t idx = hashfn(key) % HASH_SIZE;
	struct gate *g = c->table + idx;
	while (g->name[0] != 0 && strcmp(g->name, key))
	{
		if (++idx == HASH_SIZE)
		{
			idx = 0;
		}
		g = c->table + idx;
	}
	*pos = idx;
	return g->name[0] != 0;
}

static int circuit_add(struct circuit *c, struct gate *g)
{
	size_t pos;
	if (circuit_find(c, g->name, &pos))
	{
		return -1;
	}

	c->table[pos] = *g;
	c->count++;
	return 0;
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
		if (strcmp(tok, "AND") == 0)
		{
			g->op = AND;
		}
		else if (strcmp(tok, "OR") == 0)
		{
			g->op = OR;
		}
		else if (strcmp(tok, "NOT") == 0)
		{
			g->op = NOT;
		}
		else if (strcmp(tok, "LSHIFT") == 0)
		{
			g->op = LSHIFT;
		}
		else if (strcmp(tok, "RSHIFT") == 0)
		{
			g->op = RSHIFT;
		}
		else if (strcmp(tok, "->") == 0)
		{
			dst = 1;
		}
		else if (dst)
		{
			strcpy(g->name, tok);
			break;
		}
		else if (g->src1[0] == 0)
		{
			strcpy(g->src1, tok);
		}
		else
		{
			strcpy(g->src2, tok);
		}
	}
}

static struct circuit *circuit_load(FILE *input)
{
	struct circuit *c = calloc(1, sizeof(*c));
	if (!c)
	{
		return c;
	}

	char *line = NULL;
	size_t sline = 0;
	struct gate g;
	while (getline(&line, &sline, input) != -1)
	{
		memset(&g, 0, sizeof(g));
		gate_parse(line, &g);
		circuit_add(c, &g);
	}
	free(line);

	return c;
}

static uint16_t circuit_eval(struct circuit *c, const char *value)
{
	size_t pos;
	if (!circuit_find(c, value, &pos))
	{
		return strtoul(value, NULL, 10);
	}
	struct gate *g = c->table + pos;
	if (!g->computed)
	{
		switch(g->op)
		{
		case ASSIGN:
			g->value = circuit_eval(c, g->src1);
			break;
		case AND:
			g->value = circuit_eval(c, g->src1) & circuit_eval(c, g->src2);
			break;
		case OR:
			g->value = circuit_eval(c, g->src1) | circuit_eval(c, g->src2);
			break;
		case NOT:
			g->value = ~circuit_eval(c, g->src1);
			break;
		case LSHIFT:
			g->value = circuit_eval(c, g->src1) << circuit_eval(c, g->src2);
			break;
		case RSHIFT:
			g->value = circuit_eval(c, g->src1) >> circuit_eval(c, g->src2);
			break;
		default:
			abort();
		}
		g->computed = 1;
	}
	return g->value;
}

static void circuit_reset(struct circuit *c)
{
	for (size_t i = 0; i < HASH_SIZE; i++)
	{
		c->table[i].computed = 0;
	}
}

static void circuit_override(struct circuit *c, const char *key, uint16_t value)
{
	size_t pos;
	if (circuit_find(c, key, &pos))
	{
		c->table[pos].value = value;
		c->table[pos].computed = 1;
	}
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
		fprintf(stderr, "Cannot open %s for reading\n", argv[1]);
		return 1;
	}

	struct circuit *c = circuit_load(input);
	fclose(input);
	if (!c)
	{
		fputs("Cannot parse the data\n", stderr);
		return 1;
	}

	uint16_t part1 = circuit_eval(c, "a");
	printf("Part1: %d\n", part1);

	circuit_reset(c);
	circuit_override(c, "b", part1);
	uint16_t part2 = circuit_eval(c, "a");

	printf("Part2: %d\n", part2);
	free(c);
	return 0;
}
