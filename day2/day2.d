import std.stdio;

int main(string[] args)
{
	if (args.length < 2)
	{
		stderr.writefln("Usage: %s filename", args[0]);
		return 1;
	}

	try
	{
		File input = File(args[1], "r");

		int l, w, h;
		int paper = 0;
		int ribbon = 0;
		while (input.readf(" %sx%sx%s", l, w, h) == 3)
		{
			int t;
			if (l > w) { t = l; l = w; w = t; }
			if (l > h) { t = l; l = h; h = t; }
			if (w > h) { t = w; w = h; h = t; }

			paper += 3 * l * w + 2 * h * (l + w);
			ribbon += 2 * (l + w) + l * w * h;
		}
		input.close();

		writeln("Part1: ", paper);
		writeln("Part2: ", ribbon);
	}
	catch (std.exception.ErrnoException)
	{
		stderr.writefln("Cannot open %s", args[1]);
		return 1;
	}

	return 0;
}
