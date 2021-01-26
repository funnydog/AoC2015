import std.array;
import std.file;
import std.stdio;
import std.string;
import std.format;

static char[] after(char[] cur, ulong cycles)
{
	foreach (_; 0..cycles)
	{
		char prev = cur[0];
		ulong count = 0;
		auto app = appender!(char[]);
		foreach (c; cur)
		{
			if (prev == c)
			{
				count++;
			}
			else
			{
				app ~= format("%s%c", count, prev);
				count = 1;
			}
			prev = c;
		}
		app ~= format("%s%c", count, prev);
		cur = app[];
	}
	return cur;
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
		txt = strip(cast(char[])read(args[1]));
	}
	catch (FileException f)
	{
		stderr.writeln(f.msg);
		return 1;
	}

	auto str = after(txt, 40);
	writeln("Part1: ", str.length);

	str = after(str, 10);
	writeln("Part2: ", str.length);
	return 0;
}
