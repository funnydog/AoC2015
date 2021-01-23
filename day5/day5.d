import std.algorithm.searching;
import std.stdio;
import std.range;

static bool is_nice(char[] line)
{
	static const char[][] forbidden = ["ab", "cd", "pq", "xy"];
	ulong vowels = 0;
	ulong twice_in_row = 0;
	char prev = 0;
	foreach(c; line)
	{
		if (!"aeiou".find(c).empty)
		{
			vowels++;
		}

		if (c == prev)
		{
			twice_in_row++;
		}

		if (prev && !forbidden.find([prev,c]).empty)
		{
			return 0;
		}
		prev = c;
	}
	return vowels >= 3 && twice_in_row >= 1;
}

static bool is_nice2(char[] line)
{
	bool rule1 = false;
	bool rule2 = false;
	char prev = 0;
	foreach (i, c; line)
	{
		if (!rule1 && i >= 1)
		{
			rule1 = !line[i+1 .. $].find([prev, c]).empty;
		}
		if (!rule2 && i >= 2)
		{
			rule2 = (c == line[i-2]);
		}
		if (rule1 && rule2)
		{
			break;
		}
		prev = c;
	}
	return rule1 && rule2;
}

int main(string[] args)
{
	if (args.length < 2)
	{
		stderr.writefln("Usage: %s <filename>", args[0]);
		return 1;
	}

	ulong part1 = 0;
	ulong part2 = 0;
	try
	{
		File input = File(args[1], "rb");
		foreach (line; input.byLine)
		{
			if (is_nice(line)) part1++;
			if (is_nice2(line)) part2++;
		}
		input.close();
	}
	catch(std.exception.ErrnoException)
	{
		stderr.writefln("Cannot open %s", args[1]);
		return 1;
	}

	writeln("Part1: ", part1);
	writeln("Part2: ", part2);
	return 0;
}
