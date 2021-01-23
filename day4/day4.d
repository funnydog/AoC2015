import std.digest.md;
import std.file;
import std.format;
import std.stdio;
import std.string;

static ulong find_number(alias pred)(char[] salt)
	if (is(typeof(pred(cast(ubyte[])salt)) == bool))
{
	char[100] buf;
	MD5 md5;
	for (ulong i = 0; ; i++)
	{
		md5.start();
		md5.put(cast(ubyte[])(sformat!"%s%s"(buf, salt, i)));
		auto hash = md5.finish();
		if (pred(hash))
		{
			return i;
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
		txt = strip(cast(char[])read(args[1]));
	}
	catch(std.file.FileException)
	{
		stderr.writeln("Cannot open ", args[1]);
		return 1;
	}

	static bool check_five(ubyte[] hash)
	{
		return (hash[0]|hash[1]|(hash[2]&0xf0)) == 0;
	}

	static bool check_six(ubyte[] hash)
	{
		return (hash[0]|hash[1]|hash[2]) == 0;
	}

	writeln("Part1: ", find_number!(check_five)(txt));
	writeln("Part2: ", find_number!(check_six)(txt));
	return 0;
}
