import std.algorithm.mutation;
import std.conv;
import std.file;
import std.regex;
import std.stdio;

static void enlarge(ref int[][] adj)
{
	foreach(ref a; adj)
	{
		a.length++;
	}
	adj ~= new int[adj.length+1];
}

static int[][] parse(in string txt)
{
	ulong[string] idx;
	int[][] adj;
	auto pat = regex(r"(\w+) would (gain|lose) (\d+) happiness units by sitting next to (\w+).");
	foreach (m; matchAll(txt, pat))
	{
		auto one = m[1];
		auto happiness = to!int(m[3]);
		if (m[2] != "gain")
		{
			happiness = -happiness;
		}
		auto other = m[4];
		if (!(one in idx))
		{
			idx[one] = idx.length;
			enlarge(adj);
		}
		if (!(other in idx))
		{
			idx[other] = idx.length;
			enlarge(adj);
		}
		adj[idx[one]][idx[other]] = happiness;
	}
	return adj;
}

static int happiness(in int[][] adj, in ulong[] a)
{
	assert(a.length >= 2);
	ulong l = a[$-2];
	ulong i = a[$-1];
	int h = 0;
	foreach (r; a)
	{
		h += adj[i][l];
		h += adj[i][r];
		l = i;
		i = r;
	}
	return h;
}

static int permutation(in int[][] adj, ulong[] a, ulong l, ulong r)
{
	if (l == r)
	{
		return happiness(adj, a);
	}
	else
	{
		int best = int.min;
		foreach (i; l..r+1)
		{
			swap(a[l], a[i]);
			auto h = permutation(adj, a, l+1, r);
			if (best < h)
			{
				best = h;
			}
			swap(a[l], a[i]);
		}
		return best;
	}
}

static int maximize(in int[][] adj)
{
	ulong[] a = new ulong[adj.length];
	foreach (i; 0..a.length)
	{
		a[i] = i;
	}
	return permutation(adj, a, 0, a.length-1);
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

	auto adj = parse(txt);
	writeln("Part1:", maximize(adj));

	enlarge(adj);
	writeln("Part2:", maximize(adj));
	return 0;
}
