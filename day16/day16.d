import std.conv;
import std.file;
import std.regex;
import std.stdio;

static int[string][] parse(string txt)
{
	auto r = regex(r"(\w+):? (\d+),?");
	int[string][] lst;
	int[string] aunt;
	foreach (m; matchAll(txt, r))
	{
		if (m[1] == "Sue" && aunt)
		{
			lst ~= aunt;
			aunt = null;
		}
		aunt[m[1]] = to!int(m[2]);
	}
	lst ~= aunt;
	return lst;
}

static int[string][] remove_if_ne(int[string][] aunts, string prop, int value)
{
	int[string][] lst;
	ulong i = aunts.length;
	while (i-->0)
	{
		if (!(prop in aunts[i]) || aunts[i][prop] == value)
		{
			lst ~= aunts[i];
		}
	}
	return lst;
}

static int[string][] remove_if_lte(int[string][] aunts, string prop, int value)
{
	int[string][] lst;
	ulong i = aunts.length;
	while (i-->0)
	{
		if (!(prop in aunts[i]) || aunts[i][prop] > value)
		{
			lst ~= aunts[i];
		}
	}
	return lst;
}

static int[string][] remove_if_gte(int[string][] aunts, string prop, int value)
{
	int[string][] lst;
	ulong i = aunts.length;
	while (i-->0)
	{
		if (!(prop in aunts[i]) || aunts[i][prop] < value)
		{
			lst ~= aunts[i];
		}
	}
	return lst;
}

static void solve(in string txt, out int part1, out int part2)
{
	auto aunts1 = parse(txt);
	auto aunts2 = aunts1;
	struct rule {
		string property;
		int value;
		int[string][] function (int[string][], string, int) fn;
	}
	const rule[] rules = [
		rule("children", 3, &remove_if_ne),
		rule("cats", 7, &remove_if_lte),
		rule("samoyeds", 2, &remove_if_ne),
		rule("pomeranians", 3, &remove_if_gte),
		rule("akitas", 0, &remove_if_ne),
		rule("vizslas", 0, &remove_if_ne),
		rule("goldfish", 5, &remove_if_gte),
		rule("trees", 3, &remove_if_lte),
		rule("cars", 2, &remove_if_ne),
		rule("perfumes", 1, &remove_if_ne),
		];
	foreach (r; rules)
	{
		aunts1 = remove_if_ne(aunts1, r.property, r.value);
		aunts2 = r.fn(aunts2, r.property, r.value);
	}
	part1 = aunts1.length == 0 ? 0 : aunts1[0]["Sue"];
	part2 = aunts2.length == 0 ? 0 : aunts2[0]["Sue"];
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
	catch(FileException e)
	{
		stderr.writeln(e.msg);
		return 1;
	}

	int part1, part2;
	solve(txt, part1, part2);
	writeln("Part1: ", part1);
	writeln("Part2: ", part2);
	return 0;
}
