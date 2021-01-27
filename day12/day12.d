import std.conv;
import std.file;
import std.regex;
import std.stdio;

static int sum_integers(R)(RegexMatch!R tokens)
{
	int sum = 0;
	foreach (t; tokens)
	{
		if (t.whichPattern == 3)
		{
			sum += to!int(t[0]);
		}
	}
	return sum;
}

static void match(R)(ref RegexMatch!R token, string txt)
{
	assert(token.front.hit == txt,
	       "Malformed input: expected " ~ txt ~ " got " ~ token.front.hit);
	token.popFront();
}

static int sum_recursive(R)(ref RegexMatch!R token)
{
	if (token.empty)
	{
		return 0;
	}
	else if (token.front.whichPattern == 3)
	{
		auto t = to!int(token.front.hit);
		token.popFront();
		return t;
	}
	else if (token.front.whichPattern == 2)
	{
		token.popFront();
		return 0;
	}
	else if (token.front.hit == "[")
	{
		int v = 0;
		do {
			token.popFront();
			v += sum_recursive(token);
		} while (token.front.hit == ",");
		match(token, "]");
		return v;
	}
	else if (token.front.hit == "{")
	{
		int v = 0;
		bool ignore = false;
		do
		{
			token.popFront();
			token.popFront(); // skip the key

			match(token, ":");

			if (token.front.hit == `"red"`)
			{
				ignore = true;
				token.popFront();
			}
			else
			{
				v += sum_recursive(token);
			}
		} while (token.front.hit == ",");
		match(token, "}");
		return ignore ? 0 : v;
	}
	else
	{
		assert(0, "Unreachable code");
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

	auto pat = regex([`[\[\]{},:]`, `"[^"]*"`, `-?\d+`]);
	auto tokens = matchAll(txt, pat);
	writeln("Part1: ", sum_integers(tokens));
	writeln("Part2: ", sum_recursive(tokens));
	return 0;
}
