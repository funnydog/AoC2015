import std.file;
import std.stdio;
import std.string;

struct pos
{
	int x, y;

	void move(char d)
	{
		switch (d)
		{
		case '^': y--; break;
		case '>': x++; break;
		case 'v': y++; break;
		case '<': x--; break;
		default: assert(0, "Unknown direction " ~ d);
		}
	}
}

int main(string[] args)
{
	if (args.length < 2)
	{
		stderr.writefln("Usage: %s <filename>", args[0]);
		return 1;
	}

	char[] txt;
	try
	{
		txt = strip(cast(char[])(read(args[1])));
	}
	catch (FileException)
	{
		stderr.writefln("Cannot open %s", args[1]);
		return 1;
	}

	pos santa = pos(0,0);
	int[pos] map;
	map[santa] = 1;
	foreach (d; txt)
	{
		santa.move(d);
		map[santa]++;
	}
	writeln("Part1: ", map.length);

	santa = pos(0,0);
	auto robosanta = pos(0,0);
	map.clear();
	map[santa] = 2;
	foreach(i, d; txt)
	{
		if (i % 2 == 0)
		{
			santa.move(d);
			map[santa]++;
		}
		else
		{
			robosanta.move(d);
			map[robosanta]++;
		}
	}
	writeln("Part2: ", map.length);
	return 0;
}
