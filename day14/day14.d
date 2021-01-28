import std.algorithm.searching;
import std.conv;
import std.file;
import std.regex;
import std.stdio;

struct reindeer
{
	int speed;
	int flight;
	int rest;

	int distance(int seconds) const
	{
		int q = seconds / (flight + rest);
		int r = seconds % (flight + rest);
		if (r > flight)
		{
			r = flight;
		}
		return speed * (q * flight + r);
	}
}

static reindeer[] parse(in string txt)
{
	auto pat = regex(r"\w+ can fly (\d+) km/s for (\d+) seconds, but then must rest for (\d+) seconds.");
	reindeer[] r;
	foreach (m; matchAll(txt, pat))
	{
		int speed = to!int(m[1]);
		int flight = to!int(m[2]);
		int rest = to!int(m[3]);
		r ~= reindeer(speed, flight, rest);
	}
	return r;
}

static int max_distance(in reindeer[] reindeers, int seconds)
{
	int max = int.min;
	foreach(ref r; reindeers)
	{
		int d = r.distance(seconds);
		if (max < d)
		{
			max = d;
		}
	}
	return max;
}

static int max_points(in reindeer[] reindeers, int seconds)
{
	int[] points = new int[reindeers.length];
	int[] distance = new int[reindeers.length];
	for (int t = 1; t <= seconds; t++)
	{
		int max = int.min;
		foreach (i, ref r; reindeers)
		{
			distance[i] = r.distance(t);
			if (max < distance[i])
			{
				max = distance[i];
			}
		}
		foreach(i, d; distance)
		{
			if (max == d)
			{
				points[i]++;
			}
		}
	}
	return maxElement(points);
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

	auto reindeers = parse(txt);
	writeln("Part1: ", max_distance(reindeers, 2503));
	writeln("Part2: ", max_points(reindeers, 2503));
	return 0;
}
