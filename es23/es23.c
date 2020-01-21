#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

enum
{
	OP_HLF,
	OP_TPL,
	OP_INC,
	OP_JMP,
	OP_JIE,
	OP_JIO,
};

struct ins
{
	int op;
	int reg;
	int offset;
};

static struct ins *program_load(FILE *input, size_t *count)
{
	struct ins *data = NULL;
	size_t dsize = 0;
	size_t dcount = 0;
	char *line = NULL;
	size_t sline = 0;
	while (getline(&line, &sline, input) != -1)
	{
		char reg = 0;
		int offset = 0;
		int op;
		if (sscanf(line, " hlf %c", &reg) == 1)
		{
			op = OP_HLF;
		}
		else if (sscanf(line, " tpl %c", &reg) == 1)
		{
			op = OP_TPL;
		}
		else if (sscanf(line, " inc %c", &reg) == 1)
		{
			op = OP_INC;
		}
		else if (sscanf(line, " jmp %d", &offset) == 1)
		{
			op = OP_JMP;
		}
		else if (sscanf(line, " jie %c, %d", &reg, &offset) == 2)
		{
			op = OP_JIE;
		}
		else if (sscanf(line, " jio %c, %d", &reg, &offset) == 2)
		{
			op = OP_JIO;
		}
		else
		{
			fprintf(stderr, "skipped line: %s\n", line);
			abort();
		}
		if (dsize == dcount)
		{
			size_t nsize = dsize ? dsize * 2 : 16;
			struct ins *ndata = realloc(data, nsize * sizeof(*ndata));
			assert(ndata);
			dsize = nsize;
			data = ndata;
		}
		data[dcount].op = op;
		data[dcount].reg = reg - 'a';
		data[dcount].offset = offset;
		dcount++;
	}
	free(line);

	*count = dcount;
	return data;
}

struct system
{
	size_t pc;
	int reg[2];
};

static void program_execute(struct system *s, struct ins *p, size_t pcount)
{
	while (s->pc < pcount)
	{
		struct ins *i = p + s->pc;
		switch (i->op)
		{
		case OP_HLF:
			s->reg[i->reg] /= 2;
			s->pc++;
			break;

		case OP_TPL:
			s->reg[i->reg] *= 3;
			s->pc++;
			break;

		case OP_INC:
			s->reg[i->reg]++;
			s->pc++;
			break;

		case OP_JMP:
			s->pc += i->offset;
			break;

		case OP_JIE:
			if (s->reg[i->reg] % 2 == 0)
			{
				s->pc += i->offset;
			}
			else
			{
				s->pc++;
			}
			break;

		case OP_JIO:
			if (s->reg[i->reg] == 1)
			{
				s->pc += i->offset;
			}
			else
			{
				s->pc++;
			}
			break;
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
		fprintf(stderr, "Cannot open %s for loading\n", argv[1]);
		return -1;
	}

	size_t pcount = 0;
	struct ins *program = program_load(input, &pcount);
	fclose(input);

	struct system s = {};
	program_execute(&s, program, pcount);
	printf("part1: %d\n", s.reg[1]);

	s.pc = 0;
	s.reg[0] = 1;
	s.reg[1] = 0;
	program_execute(&s, program, pcount);
	printf("part2: %d\n", s.reg[1]);

	free(program);
	return 0;
}
