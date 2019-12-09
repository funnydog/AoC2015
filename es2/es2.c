#include <stdio.h>

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

	int ribbon = 0;
	int area = 0;
	int l, w, h;
	while (fscanf(input, " %dx%dx%d", &l, &w, &h) == 3) {
		int s1 = l*w;
		int s2 = w*h;
		int s3 = h*l;
		area += 2*(s1+s2+s3);
		if (s2 < s1)
			s1 = s2;
		if (s3 < s1)
			s1 = s3;
		area += s1;

		int p1 = 2*(l+w);
		int p2 = 2*(w+h);
		int p3 = 2*(h+l);
		if (p2 < p1)
			p1 = p2;
		if (p3 < p1)
			p1 = p3;
		ribbon += p1;
		ribbon += l*w*h;
	}
	fclose(input);

	printf("Total square feet of wrapping paper: %d\n", area);
	printf("Total feet of ribbon: %d\n", ribbon);
	return 0;
}
