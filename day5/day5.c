#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int isnice(const char *str)
{
	int vovels = 0;
	int previous = 0;
	int twiceinrow = 0;

	const char *forbidden[] = {"ab", "cd", "pq", "xy"};
	int indices[4] = {0};
	for (; *str; str++)
	{
		if (strchr("aeiou", *str))
		{
			vovels++;
		}

		if (*str == previous)
		{
			twiceinrow++;
		}

		for (int i = 0; i < 4; i++)
		{
			if (*str == forbidden[i][indices[i]])
			{
				indices[i]++;
				if (forbidden[i][indices[i]] == 0)
				{
					return 0;
				}
			}
			else
			{
				indices[i] = 0;
			}
		}

		previous = *str;
	}

	if (vovels < 3)
	{
		return 0;
	}

	if (twiceinrow < 1)
	{
		return 0;
	}

	return 1;
}

int isnice2(const char *str)
{
	int rule1 = 0;
	int rule2 = 0;

	char seq[3] = {0};
	for (size_t i = 0; str[i]; i++)
	{
		if (!rule1)
		{
			seq[0] = seq[1];
			seq[1] = str[i];
			char *find = strstr(str + i + 1, seq);
			if (seq[0] && find)
			{
				rule1 = 1;
			}
		}

		if (!rule2)
		{
			if (i >= 2 && str[i-2] == str[i])
			{
				rule2 = 1;
			}
		}

		if (rule1 && rule2)
		{
			break;
		}
	}

	return rule1 && rule2;
}

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

	assert(isnice("ugknbfddgicrmopn"));
	assert(isnice("aaa"));
	assert(!isnice("jchzalrnumimnmhp"));
	assert(!isnice("haegwjzuvuyypxyu"));
	assert(!isnice("dvszwmarrgswjxmb"));

	assert(isnice2("qjhvhtzxzqqjkmpb"));
	assert(isnice2("xxyxx"));
	assert(!isnice2("uurcxstgmygtbstg"));
	assert(!isnice2("ieodomkazucvgmuy"));

	size_t part1 = 0;
	size_t part2 = 0;
	char *line = NULL;
	size_t linesize = 0;
	while (getline(&line, &linesize, input) != -1)
	{
		line[strlen(line)-1] = 0;
		if (isnice(line))
		{
			part1++;
		}
		if (isnice2(line))
		{
			part2++;
		}
	}
	free(line);
	fclose(input);

	printf("part1: %zu\n", part1);
	printf("part2: %zu\n", part2);
	return 0;
}
