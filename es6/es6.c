#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
	NONE = 0,
	TURN_ON,
	TOGGLE,
	TURN_OFF,
};

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
		return -1;
	}

	FILE *input = fopen(argv[1], "rb");
	if (!input) {
		fprintf(stderr, "Cannot open %s for reading\n", argv[1]);
		return -1;
	}

	int grid[1000][1000] = {0};
	char *line = NULL;
	size_t linesize = 0;
	while (getline(&line, &linesize, input) >= 0) {
		int x0, x1, y0, y1;
		int action = 0;
		int skip = 0;
		if (strstr(line, "turn on ") == line) {
			action = TURN_ON;
			skip = strlen("turn on ");
		} else if (strstr(line, "toggle ") == line) {
			action = TOGGLE;
			skip = strlen("toggle ");
		} else if (strstr(line, "turn off ") == line) {
			action = TURN_OFF;
			skip = strlen("turn off ");
		}
		sscanf(line+skip, "%d,%d through %d,%d\n", &x0, &y0, &x1, &y1);
		switch (action) {
		case TURN_ON:
			for (int y = y0; y <= y1; y++)
				for (int x = x0; x <= x1; x++)
					grid[y][x]++;
			break;
		case TOGGLE:
			for (int y = y0; y <= y1; y++)
				for (int x = x0; x <= x1; x++)
					grid[y][x] += 2;
			break;

		case TURN_OFF:
			for (int y = y0; y <= y1; y++)
				for (int x = x0; x <= x1; x++)
					if (grid[y][x] > 0)
						grid[y][x]--;
			break;

		default:
			break;
		}
	}
	free(line);

	int count = 0;
	for (int y = 0; y < 1000; y++)
		for (int x = 0; x < 1000; x++)
			count += grid[y][x];

	printf("lit lights: %d\n", count);

	fclose(input);
	return 0;
}
