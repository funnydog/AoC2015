#include <assert.h>
#include <limits.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

static int sum_integers(FILE *input)
{
	int sum = 0;
	int c;
	while ((c = getc(input)) != EOF)
	{
		if (c == '-' || isdigit(c))
		{
			ungetc(c, input);
			int value;
			if (fscanf(input, "%d", &value) != 1)
			{
				abort();
			}
			sum += value;
		}
	}
	return sum;
}

static int recursive_sum(FILE *input, int *is_red)
{
	int c = getc(input);
	if (c == EOF)
	{
		return 0;
	}
	else if (isdigit(c) || c == '-')
	{
		int sum;
		ungetc(c, input);
		if (fscanf(input, "%d", &sum) != 1)
		{
			abort();
		}
		return sum;
	}
	else if (c == '"')
	{
		/* read strings */
		const char *red = "red";
		while ((c = getc(input)) != '"')
		{
			if (c == EOF)
			{
				abort();
			}
			if (c == red[0])
			{
				red++;
			}
		}
		if (*red == 0 && is_red)
		{
			*is_red = 1;
		}

		return 0;
	}
	else if (c == '[')
	{
		/* read array */
		int sum = 0;
		do
		{
			sum += recursive_sum(input, NULL);
		} while ((c = getc(input)) == ',');
		if (c != ']')
		{
			abort();
		}
		return sum;
	}
	else if (c == '{')
	{
		/* read object */
		int sum = 0;
		int is_property_red = 0;
		do
		{
			sum += recursive_sum(input, NULL);
			c = getc(input);
			assert(c == ':');
			sum += recursive_sum(input, &is_property_red);
		} while ((c = getc(input)) == ',');

		assert(c == '}');

		if (is_property_red)
		{
			return 0;
		}

		return sum;
	}

	return 0;
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
		return -1;
	}

	printf("Part1: %d\n", sum_integers(input));
	rewind(input);
	printf("Part2: %d\n", recursive_sum(input, NULL));
	fclose(input);
	return 0;
}
