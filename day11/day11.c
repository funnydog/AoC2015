#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int is_good(const char *s)
{
	int rule1 = 0;
	int rule3 = 0;
	char last = 0;
	const char *pos = s;
	for (; *s; s++)
	{
		/* rule2: 'i' 'o' 'l' forbidden */
		if (strchr("iol", *s))
		{
			return 0;
		}

		/* rule1: increasing straight of at least 3 letters */
		if (rule1 < 2)
		{
			rule1 = (*s == last+1) ? (rule1+1) : 0;
		}

		/* rule3: non-overlapping pairs of letters */
		if (*s == last && s - pos < 2)
		{
			rule3++;
		}
		else
		{
			pos = s;
		}
		last = *s;
	}
	return rule1 >= 2 && rule3 >= 2;
}

static int next_password(char *pw, size_t len)
{
	size_t i = len;
	while (i-->0)
	{
		if (pw[i] != 'z')
		{
			pw[i]++;
			if (is_good(pw))
			{
				return 1;
			}
			/* restart */
			i = len;
		}
		else
		{
			pw[i] = 'a';
		}
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
		return 1;
	}

	size_t spassword = 0;
	char *password = NULL;
	int r = getline(&password, &spassword, input);
	fclose(input);
	if (r == -1)
	{
		perror("Cannot parse the data");
		return 1;
	}

	/* right trim */
	size_t len = strlen(password);
	while (len > 0 && isspace(password[len-1]))
	{
		password[--len] = 0;
	}

	if (next_password(password, len))
	{
		printf("Part1: %s\n", password);
	}
	if (next_password(password, len))
	{
		printf("Part2: %s\n", password);
	}
	free(password);
	return 0;
}
