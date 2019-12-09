#include <stdio.h>
#include <string.h>

static int check_pw(const char *s)
{
	int rule1 = 0;
	int rule2 = 0;
	char last = 0;
	const char *pos = NULL;
	for (; s[0]; s++) {
		if (strchr("iol", s[0]))
			return 0;

		if (rule1 < 2) {
			if (s[0] == last + 1) {
				rule1++;
			} else {
				rule1 = 0;
			}
		}
		if (rule2 == 0 && s[0] == last) {
			pos = s;
			rule2++;
		} else if (rule2 == 1 && s-pos > 1 && s[0] == last) {
			rule2++;
		}
		last = s[0];
	}
	return rule1 >= 2 && rule2 >= 2;
}

static int make_next(char *pw)
{
	size_t len = strlen(pw);
	while (len-- > 0) {
		if (pw[len] != 'z') {
			pw[len]++;
			if (check_pw(pw))
				return 1;
			len = strlen(pw);
		} else {
			pw[len] = 'a';
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <password>\n", argv[0]);
		return -1;
	}

	for (int i = 0; i < 2; i++) {
		if (!make_next(argv[1]))
			break;
		printf("next password %s\n", argv[1]);
	}

	return 0;
}
