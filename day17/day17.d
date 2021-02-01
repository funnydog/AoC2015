import std.stdio;

static int part1(in int[] containers)
{
	auto table = new int[150+1];
	table[0] = 1;
	foreach(cap; containers)
	{
		int s = 150+1;
		while (s-- > cap)
		{
			table[s] += table[s - cap];
		}
	}
	return table[150];
}

static int part2(in int[] containers)
{
	auto table = new int[][][](containers.length+1, containers.length+1, 150+1);
	foreach (i; 0..containers.length+1)
	{
		table[0][i][0] = 1;
	}
	foreach (count; 1..containers.length+1)
	{
		foreach (j; 1..containers.length+1)
		{
			int cap = containers[j-1];
			foreach(s; 1..151)
			{
				table[count][j][s] = table[count][j-1][s];
				if (s >= cap)
				{
					table[count][j][s] += table[count-1][j-1][s-cap];
				}
			}
		}
		auto t = table[count][containers.length][150];
		if (t)
		{
			return t;
		}
	}
	return -1;
}

int main(string[] args)
{
	if (args.length < 2)
	{
		stderr.writefln("Usage: %s <filename>", args[0]);
		return 1;
	}

	int[] containers;
	try
	{
		File input = File(args[1], "rb");
		int value;
		while (input.readf(" %s", value) == 1)
		{
			containers ~= value;
		}
		input.close();
	}
	catch (std.exception.ErrnoException e)
	{
		stderr.writeln(e.msg);
		return 1;
	}

	writeln("Part1: ", part1(containers));
	writeln("Part2: ", part2(containers));
	return 0;
}
