#include <assert.h>
#include <stdio.h>
#include <stdint.h>

static int get_value(int x, int y)
{
	assert(x > 0 && y > 0);
	int ny = y + x - 1;
	return ny * (ny-1) / 2 + x;
}

static uint32_t mulmod(uint32_t a, uint32_t b, uint32_t m)
{
	assert((a & 0x80000000) == 0);
	assert((b & 0x80000000) == 0);;
	a %= m;
	b %= m;
	uint32_t result = 0;
	while (b > 0)
	{
		if (b & 1)
		{
			result = (result + a) % m;
		}
		b >>= 1;
		a = (a * 2) % m;
	}
	return result;
}

static uint32_t powmod(uint32_t b, uint32_t e, uint32_t m)
{
	assert((b & 0x80000000) == 0);
	assert((e & 0x80000000) == 0);;
	if (m == 1)
	{
		return 0;
	}
	b %= m;
	uint32_t r = 1;
	while (e > 0)
	{
		if (e & 1)
		{
			r = mulmod(r, b, m);
		}
		e >>= 1;
		b = mulmod(b, b, m);
	}
	return r;
}

static uint32_t gen_code(int pos)
{
	assert(pos > 0);
	return mulmod(
		20151125,
		powmod(252533, pos-1, 33554393),
		33554393);
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
		fprintf(stderr, "Cannot open %s for reading\n", argv[1]);
		return -1;
	}

	int x, y;
	int r = fscanf(
		input, "To continue, please consult the code grid in the"
		" manual.  Enter the code at row %d, column %d.", &y, &x);
	fclose(input);

	if (r != 2)
	{
		fprintf(stderr, "Cannot load the input\n");
		return -1;
	}

	printf("part1: %u\n", gen_code(get_value(x, y)));

	return 0;
}
