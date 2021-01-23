import std.conv;
import std.file;
import std.regex;
import std.stdio;

enum Action
{
	TURN_ON,
	TURN_OFF,
	TOGGLE
}

static ulong count_lights(alias fn)(string line)
{
	auto table = new int[][](1000, 1000);
	static const auto r = regex(r"(turn on|turn off|toggle) (\d+),(\d+) through (\d+),(\d+)");
	foreach (m; matchAll(line, r))
	{
		Action a;
		if (m[1] == "turn on")
		{
			a = Action.TURN_ON;
		}
		else if(m[1] == "turn off")
		{
			a = Action.TURN_OFF;
		}
		else
		{
			a = Action.TOGGLE;
		}
		auto x1 = std.conv.to!int(m[2]);
		auto y1 = std.conv.to!int(m[3]);
		auto x2 = std.conv.to!int(m[4]);
		auto y2 = std.conv.to!int(m[5]);
		foreach (y; y1 .. y2 +1)
		{
			foreach (x; x1 .. x2 + 1)
			{
				fn(table[y][x], a);
			}
		}
	}
	ulong count = 0;
	foreach(row; table)
	{
		foreach (v; row)
		{
			count += v;
		}
	}
	return count;
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
	catch (FileException)
	{
		stderr.writefln("Cannot open %s", args[1]);
		return 1;
	}

	static void part1(ref int x, Action a)
	{
		switch (a)
		{
		case Action.TURN_ON: x = 1; break;
		case Action.TURN_OFF: x = 0; break;
		case Action.TOGGLE: x = 1 - x; break;
		default: assert(0, "Unknown action");
		}
	}
	writeln("Part1: ", count_lights!part1(txt));

	static void part2(ref int x, Action a)
	{
		switch (a)
		{
		case Action.TURN_ON: x++; break;
		case Action.TURN_OFF: if (x > 0) x--; break;
		case Action.TOGGLE: x += 2; break;
		default: assert(0, "Unknown action");
		}
	}
	writeln("Part2: ", count_lights!part2(txt));
	return 0;
}
