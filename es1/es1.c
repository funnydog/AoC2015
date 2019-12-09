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


	int c;
	int pos = 0;
	int floor = 0;

	while ((c = getc(input)) != EOF) {
		switch(c) {
		case '(': floor++; break;
		case ')': floor--; break;
		default:
		break;
		}

		if (floor == -1 && pos == 0) {
			pos = ftello(input);
		}
	}

	printf("Final floor: %d\n", floor);
	printf("Position of the first char to enter -1: %d\n", pos);

	fclose(input);
	return 0;
}
