#include <stdint.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
		return 1;
	}

	FILE *input = fopen(argv[1], "rb");
	if (!input)
	{
		fprintf(stderr, "Cannot open %s\n", argv[1]);
		return 1;
	}


	int c;
	size_t pos = SIZE_MAX;
	int floor = 0;
	for (size_t i = 1; (c = getc(input)) != EOF; i++)
	{
		switch(c)
		{
		case '(': floor++; break;
		case ')': floor--; break;
		default:
		break;
		}

		if (floor == -1 && pos == SIZE_MAX)
		{
			pos = i;
		}
	}
	fclose(input);

	printf("Part1: %d\n", floor);
	printf("Part2: %zu\n", pos);
	return 0;
}
