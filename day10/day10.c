#include <ctype.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct string
{
	char *line;
	size_t count;
	size_t size;
};

static void append(struct string *s, size_t count, int c)
{
	if (s->count >= s->size)
	{
		size_t nsize = s->size ? s->size * 2 : 1024;
		char *nline = realloc(s->line, nsize);
		assert(nline);
		s->size = nsize;
		s->line = nline;
	}
	s->line[s->count++] = count + '0';
	s->line[s->count++] = c;
}

static void iterate(struct string *cur, size_t itercount)
{
	struct string other = {0};
	while (itercount-->0)
	{
		/* create the new sequence */
		other.count = 0;
		size_t count = 0;
		char prev = cur->line[0];
		for (size_t i = 0; i < cur->count; i++)
		{
			if (prev == cur->line[i])
			{
				count++;
			}
			else
			{
				append(&other, count, prev);
				count = 1;
				prev = cur->line[i];
			}
		}
		append(&other, count, prev);

		/* swap the two sequences */
		struct string t = *cur;
		*cur = other;
		other = t;
	}
	free(other.line);
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
		perror("Cannot open the file");
		return 1;
	}

	struct string s = {0};
	int r = getline(&s.line, &s.size, input);
	fclose(input);
	if (r == -1)
	{
		perror("Cannot parse the data");
		return 1;
	}

	/* right trim */
	s.count = strlen(s.line);
	while (s.count > 0 && isspace(s.line[s.count-1]))
	{
		s.line[--s.count] = 0;
	}

	iterate(&s, 40);
	printf("Part1: %zu\n", s.count);
	iterate(&s, 10);
	printf("Part2: %zu\n", s.count);
	free(s.line);
	return 0;
}
