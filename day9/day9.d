import std.algorithm.mutation;
import std.conv;
import std.file;
import std.stdio;
import std.string;

class Map
{
	int[32][32] distance;
	int[32] route;
	ulong count;

	this(string txt)
	{
		foreach(ref row; distance)
		{
			foreach(ref v; row)
			{
				v = int.max;
			}
		}
		ulong[string] idx;
		foreach (line; txt.splitLines)
		{
			auto lst = split(line, " ");
			if (lst.length > 4)
			{
				auto src = lst[0];
				if (!(src in idx))
				{
					idx[src] = count++;
				}
				auto dst = lst[2];
				if (!(dst in idx))
				{
					idx[dst] = count++;
				}
				int d = to!int(lst[4]);
				distance[idx[src]][idx[dst]] = d;
				distance[idx[dst]][idx[src]] = d;
			}
		}
	}

	int route_length() const
	{
		int d = 0;
		for (ulong i = 1; i < count; i++)
		{
			d += distance[route[i-1]][route[i]];
		}
		return d;
	}

	int shortest(ulong i, ulong last)
	{
		if (i == last)
		{
			return route_length();
		}
		else
		{
			int best = int.max;
			for (ulong j = i; j <= last; j++)
			{
				if (distance[i][j] == int.max && i != j)
				{
					continue;
				}
				swap(route[i], route[j]);
				auto cur = shortest(i+1, last);
				if (best > cur)
				{
					best = cur;
				}
				swap(route[i], route[j]);
			}
			return best;
		}
	}

	int longest(ulong i, ulong last)
	{
		if (i == last)
		{
			return route_length();
		}
		else
		{
			int best = 0;
			for (ulong j = i; j <= last; j++)
			{
				if (distance[i][j] == int.max && i != j)
				{
					continue;
				}
				swap(route[i], route[j]);
				auto cur = longest(i+1, last);
				if (best < cur)
				{
					best = cur;
				}
				swap(route[i], route[j]);
			}
			return best;
		}
	}

	int best(alias fn)()
	{
		foreach (i, ref e; route)
		{
			e = cast(int)i;
		}
		return fn(0, count-1);
	}
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

	Map m = new Map(txt);

	writeln("Part1: ", m.best!(m.shortest)());
	writeln("Part2: ", m.best!(m.longest)());

	return 0;
}
