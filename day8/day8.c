#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static size_t declength(const char *str)
{
	size_t real = 0, repr = 0;
	enum { NONE, ESCAPED, HEX1, HEX2 } state = NONE;
	for (; str[0] != 0 && str[0] != '\n'; str++)
	{
		repr++;
		if (state == NONE)
		{
			if (str[0] == '\\')
			{
				state = ESCAPED;
			}
			else if (str[0] != '"')
			{
				real++;
			}
		}
		else if (state == ESCAPED)
		{
			if (str[0] == 'x')
			{
				state = HEX1;
			}
			else if (str[0] == '\\' || str[0] == '"')
			{
				real++;
				state = NONE;
			}
			else
			{
				state = NONE;
			}
		}
		else if (state == HEX1)
		{
			if (isxdigit(str[0]))
			{
				state = HEX2;
			}
			else
			{
				real++;
				state = NONE;
			}
		}
		else if (state == HEX2)
		{
			if (isxdigit(str[0]))
			{
				real++;
			}
			state = NONE;
		}
		else
		{
			abort();
		}
	}
	return repr - real;
}

static size_t enclength(const char *s)
{
	size_t real = 0, repr = 0;
	for (; s[0] != 0 && s[0] != '\n'; s++)
	{
		real++;
		if (s[0] == '"' || s[0] == '\\')
		{
			repr += 2;
		}
		else
		{
			repr++;
		}
	}
	return repr + 2 - real;
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
		fprintf(stderr, "Cannot open %s\n", argv[1]);
		return 1;
	}

	assert(declength("\"\"") == 2-0);
	assert(declength("\"abc\"") == 5-3);
	assert(declength("\"aaa\\\"aaa\"") == 10-7);
	assert(declength("\"\\x27\"") == 6-1);

	assert(enclength("\"\"") == 6-2);
	assert(enclength("\"abc\"") == 9-5);
	assert(enclength("\"aaa\\\"aaa\"") == 16-10);
	assert(enclength("\"\\x27\"") == 11-6);

	size_t decdif = 0, encdif = 0;
	char *line;
	size_t linesize = 0;
	while (getline(&line, &linesize, input) != -1)
	{
		decdif += declength(line);
		encdif += enclength(line);
	}
	free(line);
	fclose(input);

	printf("Part1: %zu\n", decdif);
	printf("Part2: %zu\n", encdif);
	return 0;
}
