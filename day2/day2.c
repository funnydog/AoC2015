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

	int ribbon = 0;
	int area = 0;
	int l, w, h;
	while (fscanf(input, " %dx%dx%d", &l, &w, &h) == 3)
	{
		/* order the elements so that l <= w <= h */
		int t;
		if (l > w) { t = l; l = w; w = t; }
		if (l > h) { t = l; l = h; h = t; }
		if (w > h) { t = w; w = h; h = t; }

		area += 3 * l * w + 2 * h * (w + l);
		ribbon += 2 * (l + w)  + l * w * h;
	}
	fclose(input);

	printf("Part1: %d\n", area);
	printf("Part2: %d\n", ribbon);
	return 0;
}
