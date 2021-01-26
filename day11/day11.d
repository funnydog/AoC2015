import std.algorithm;
import std.file;
import std.stdio;
import std.string;

static bool is_good(in char[] pwd)
{
	ulong rule1, rule3, pos;
	char last;
	rule1 = rule3 = pos = 0;
	last = 0;

	foreach (i, c; pwd)
	{
		// rule2: iol forbidden
		if (!"iol".find(c).empty)
		{
			return false;
		}

		// rule1: three consecutive increasing letters
		if (rule1 < 2)
		{
			rule1 = (c == last+1) ? (rule1+1) : 0;
		}

		// rule3: non-overlapping pairs of letters
		if (last == c && i - pos < 2)
		{
			rule3++;
		}
		else
		{
			pos = i;
		}
		last = c;
	}

	return rule1 >= 2 && rule3 >= 2;
}

static bool next_pwd(char[] pwd)
{
	auto i = pwd.length;
	while (i-->0)
	{
		if (pwd[i] != 'z')
		{
			pwd[i]++;
			if (is_good(pwd))
			{
				return true;
			}
			i = pwd.length;
		}
		else
		{
			pwd[i] = 'a';
		}
	}

	return false;
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
	catch (FileException e)
	{
		stderr.writeln(e.msg);
		return 1;
	}

	if (next_pwd(txt))
	{
		writeln("Part1: ", txt);
	}
	if (next_pwd(txt))
	{
		writeln("Part2: ", txt);
	}
	return 0;
}
