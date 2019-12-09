#include <limits.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

static int sum_integers(FILE *input)
{
	int sum = 0;
	int c;
	while ((c = getc(input)) != EOF) {
		if (c == '-' || isdigit(c)) {
			ungetc(c, input);
			int value;
			if (fscanf(input, "%d", &value) != 1)
				abort();
			sum += value;
		}
	}
	return sum;
}

static int recursive_sum(FILE *input, int *ignore)
{
	int c = getc(input);
	if (c == EOF)
		return 0;

	if (isdigit(c) || c == '-') {
		int sum;
		ungetc(c, input);
		if (fscanf(input, "%d", &sum) != 1)
			abort();
		return sum;
	}

	if (c == '"') {
		/* read strings */
		const char *red = "red";
		while ((c = getc(input)) != '"') {
			if (c == EOF)
				abort();
			if (c == red[0])
				red++;
		}
		if (*red == 0 && ignore)
			*ignore = 1;

		return 0;
	}

	if (c == '[') {
		/* read array */
		int sum = recursive_sum(input, NULL);
		while ((c = getc(input)) == ',')
			sum += recursive_sum(input, NULL);
		if (c != ']')
			abort();

		return sum;
	}

	if (c == '{') {
		/* read object */
		int sum = 0;
		int ignore = 0;
		do {
			sum += recursive_sum(input, NULL);
			c = getc(input);
			if (c != ':')
				abort();
			sum += recursive_sum(input, &ignore);
			c = getc(input);
		} while (c == ',');
		if (c != '}')
			abort();

		if (ignore)
			return 0;

		return sum;
	}

	return 0;
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

	printf("Sum of the integers: %d\n", sum_integers(input));
	rewind(input);
	printf("Sum of the integers: %d\n", recursive_sum(input, NULL));

	fclose(input);
	return 0;
}
