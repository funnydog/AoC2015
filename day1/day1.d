import std.file;
import std.stdio;

int main(string[] args)
{
	if (args.length < 2)
	{
		stderr.writefln("Usage: %s filename", args[0]);
		return 1;
	}

	char[] txt;
	try
	{
		txt = cast(char[])(read(args[1]));
	}
	catch (std.file.FileException)
	{
		stderr.writefln("Cannot open %s", args[1]);
		return 1;
	}

	int floor = 0;
	ulong pos = ulong.max;
	foreach (i, c; txt)
	{
		if (c == '(')
		{
			floor++;
		}
		else if (c == ')')
		{
			floor--;
		}

		if (floor == -1 && pos == ulong.max)
		{
			pos = i + 1;
		}
	}
	writeln("Part1: ", floor);
	writeln("Part2: ", pos);
	return 0;
}
