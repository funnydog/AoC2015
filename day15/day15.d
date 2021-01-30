import std.conv;
import std.file;
import std.regex;
import std.stdio;

struct ingredient
{
	int[4] property;
	int calories;
}

static ingredient[] parse(in string txt)
{
	ingredient[] ings;
	auto r = regex(r"\w+ (-?\d+),?");
	int i = 0;
	ingredient t;
	foreach (m; matchAll(txt, r))
	{
		switch(i)
		{
		case 0:
		case 1:
		case 2:
		case 3: t.property[i] = to!int(m[1]); break;
		case 4: t.calories = to!int(m[1]); break;
		default: assert(0);
		}
		if (++i >= 5)
		{
			i = 0;
			ings ~= t;
		}
	}
	return ings;
}

static bool next_seq(int[] s)
{
	int sum = 0;
	foreach (i; s[0..$-1])
	{
		sum += i;
	}
	ulong right = s.length - 1;
	while (true)
	{
		right--;
		s[right]++;
		sum++;
		if (sum <= 100 || right == 0)
		{
			break;
		}
		sum -= s[right];
		s[right] = 0;
	}
	if (s[0] > 100)
	{
		return false;
	}
	s[$-1] = 100 - sum;
	return true;
}

static int score(in ingredient[] ings, int[] sol)
{
	int s = 1;
	foreach (i; 0..4)
	{
		int p = 0;
		foreach(j, ref ing; ings)
		{
			p += ing.property[i] * sol[j];
		}
		if (p <= 0)
		{
			return 0;
		}
		s *= p;
	}
	return s;
}

static int calories(in ingredient[] ings, int[] sol)
{
	int cal = 0;
	foreach (i, ref ing; ings)
	{
		cal += ing.calories * sol[i];
	}
	return cal;
}

static void solve(in string txt, out int part1, out int part2)
{
	part1 = part2 = int.min;
	auto ings = parse(txt);
	int[] sol = new int[ings.length];
	do
	{
		auto t = score(ings, sol);
		if (part1 < t)
		{
			part1 = t;
		}
		if (calories(ings, sol) == 500 && part2 < t)
		{
			part2 = t;
		}
	} while(next_seq(sol));
}

int main(string[] args)
{
	if (args.length < 2)
	{
		stderr.writefln("Usage: %s <filename>", args[0]);
		return 1;
	}

	string txt;
	try
	{
		txt = readText(args[1]);
	}
	catch (FileException e)
	{
		stderr.writeln(e.msg);
		return 1;
	}

	int part1, part2;
	solve(txt, part1, part2);
	writeln("Part1: ", part1);
	writeln("Part2: ", part2);
	return 0;
}
