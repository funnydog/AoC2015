import std.ascii;
import std.file;
import std.stdio;
import std.string;

enum State { NONE, ESCAPED, HEX1, HEX2 }

static ulong dec_diff(in string txt)
{
	ulong len = 0;
	State state = State.NONE;
	foreach(c; txt)
	{
		if (state == State.NONE)
		{
			if (c == '\\')
			{
				state = State.ESCAPED;
			}
			else if (c != '"')
			{
				len++;
			}
		}
		else if (state == State.ESCAPED)
		{
			if (c == 'x')
			{
				state = State.HEX1;
			}
			else if (c == '\\' || c == '"')
			{
				len++;
				state = State.NONE;
			}
			else
			{
				state = State.NONE;
			}
		}
		else if (state == State.HEX1)
		{
			if (isHexDigit(c))
			{
				state = State.HEX2;
			}
			else
			{
				state = State.NONE;
			}
		}
		else if (state == State.HEX2)
		{
			if (isHexDigit(c))
			{
				len++;
			}
			state = State.NONE;
		}
	}
	return txt.length - len;
}

static ulong enc_diff(in string line)
{
	ulong len = 0;
	foreach (c; line)
	{
		if (c == '\\' || c == '"')
		{
			len++;
		}
		len++;
	}
	return len + 2 - line.length;
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

	ulong part1 = 0;
	ulong part2 = 0;
	foreach(line; txt.splitLines)
	{
		part1 += dec_diff(line);
		part2 += enc_diff(line);
	}

	writeln("Part1: ", part1);
	writeln("Part2: ", part2);
	return 0;
}
