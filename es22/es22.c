#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

enum
{
	MISSILE,
	DRAIN,
	SHIELD,
	POISON,
	RECHARGE,
	MAXSPELLS,

	NONE = 0,
	WIN,
	LOSE
};

static const struct spell
{
	int cost;
	int points;
	int damage;
	int effect;
} spells[MAXSPELLS] = {
	{  53, 0, 4, 0 },
	{  73, 2, 2, 0 },
	{ 113, 0, 0, 6 },
	{ 173, 0, 0, 6 },
	{ 229, 0, 0, 5 },
};

struct game
{
	int player;
	int spent_mana;
	int mana;
	int armor;
	int boss;
	int damage;
	int hard;
	int effect[MAXSPELLS];
};

static void init_game(struct game *g, int player, int mana, int boss, int damage, int hard)
{
	g->player = player;
	g->spent_mana = 0;
	g->mana = mana;
	g->armor = 0;
	g->boss = boss;
	g->damage = damage;
	g->hard = hard;
	for (int i = 0; i < MAXSPELLS; i++)
	{
		g->effect[i] = 0;
	}
}

static void update_effects(struct game *g)
{
	for (int i = 0; i < MAXSPELLS; i++)
	{
		if (g->effect[i] == 0)
		{
			continue;
		}
		else if (i == SHIELD && g->effect[i] == 1)
		{
			g->armor -= 7;
		}
		else if (i == POISON)
		{
			g->boss -= 3;
		}
		else if (i == RECHARGE)
		{
			g->mana += 101;
		}
		g->effect[i]--;
	}
}

static int exec_game(struct game *g, int *moves, size_t mcount)
{
	for (size_t i = 0; i < mcount; i++)
	{
		if (g->hard && --g->player <= 0)
		{
			return LOSE;
		}

		update_effects(g);
		if (g->boss <= 0)
		{
			return WIN;
		}

		const struct spell *s = spells + moves[i];
		assert(s->cost <= g->mana);
		assert(g->effect[moves[i]] == 0);

		g->spent_mana += s->cost;
		g->mana -= s->cost;
		g->player += s->points;
		g->boss -= s->damage;
		g->effect[moves[i]] = s->effect;
		if (moves[i] == SHIELD)
		{
			g->armor += 7;
		}

		if (g->boss <= 0)
		{
			return WIN;
		}

		/* boss turn */
		update_effects(g);
		if (g->boss <= 0)
		{
			return WIN;
		}

		g->player -= (g->damage > g->armor) ? (g->damage - g->armor) : 1;
		if (g->player <= 0)
		{
			return LOSE;
		}
	}
	return NONE;
}

static void allowed_moves(struct game *g, int moves[], int *count)
{
	*count = 0;
	for (int i = 0; i < MAXSPELLS; i++)
	{
		if (spells[i].cost <= g->mana && g->effect[i] <= 1)
		{
			moves[*count] = i;
			*count += 1;
		}
	}
}

static void parse_parameters(FILE *input, int *points, int *damage)
{
	char *line = NULL;
	size_t lsize = 0;
	while (getline(&line, &lsize, input) != -1)
	{
		sscanf(line, " Hit Points: %d", points);
		sscanf(line, " Damage: %d", damage);
	}
	free(line);
}

int best;

static void backtrack(int moves[], size_t k, int points, int damage, int hard)
{
	struct game g;
	init_game(&g, 50, 500, points, damage, hard);
	int r = exec_game(&g, moves, k);
	if (r == WIN && best > g.spent_mana)
	{
		best = g.spent_mana;
	}
	else if (r == NONE && best > g.spent_mana)
	{
		int allowed[5];
		int count;
		allowed_moves(&g, allowed, &count);
		for (int i = 0; i < count; i++)
		{
			moves[k] = allowed[i];
			backtrack(moves, k+1, points, damage, hard);
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <input>\n", argv[0]);
		return -1;
	}

	FILE *input = fopen(argv[1], "rb");
	if (!input)
	{
		fprintf(stderr, "Cannot open %s for reading\n", argv[1]);
		return -1;
	}

	int points, damage;
	parse_parameters(input, &points, &damage);
	fclose(input);

	struct game g;
	init_game(&g, 10, 250, 13, 8, 0);
	assert(exec_game(&g, (int[]){3, 0}, 2) == WIN);
	init_game(&g, 10, 250, 14, 8, 0);
	assert(exec_game(&g, (int[]){4, 2, 1, 3, 0}, 5) == WIN);

	int moves[(500+53-1)/53];
	best = INT_MAX;
	backtrack(moves, 0, points, damage, 0);
	printf("part1: %d\n", best);

	best = INT_MAX;
	backtrack(moves, 0, points, damage, 1);
	printf("part2: %d\n", best);

	return 0;
}
