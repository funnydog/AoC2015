import std.conv;
import std.stdio;
import std.string;

enum Op
{
	ASSIGN,
	NOT,
	AND,
	OR,
	LSHIFT,
	RSHIFT,
}

class Gate
{
	string arg1;
	string arg2;
	string op;
	uint value;
	bool computed = false;
}

static Gate[string] parse(ref File input)
{
	Gate[string] gates;
	foreach(line; input.byLine)
	{
		line = strip(line);
		auto lst = line.split(" -> ");
		auto ops = lst[0].split(" ");

		Gate g = new Gate;
		switch (ops.length)
		{
		case 1:
			g.op = "ASSIGN";
			g.arg1 = ops[0].idup;
			break;
		case 2:
			g.op = ops[0].idup;
			g.arg1 = ops[1].idup;
			break;
		case 3:
			g.op = ops[1].idup;
			g.arg1 = ops[0].idup;
			g.arg2 = ops[2].idup;
			break;
		default:
			assert(0, "Unparsable line " ~ line);
		}

		gates[lst[1].idup] = g;
	}
	return gates;
}

static uint eval(Gate[string] gates, string name)
{
	if (!(name in gates))
	{
		return to!uint(name);
	}

	auto g = gates[name];
	if (!g.computed)
	{
		if (g.op == "ASSIGN")
		{
			g.value = eval(gates, g.arg1);
		}
		else if  (g.op == "NOT")
		{
			g.value = ~eval(gates, g.arg1);
		}
		else if (g.op == "AND")
		{
			g.value = eval(gates, g.arg1) & eval(gates, g.arg2);
		}
		else if (g.op == "OR")
		{
			g.value = eval(gates, g.arg1) | eval(gates, g.arg2);
		}
		else if (g.op == "LSHIFT")
		{
			g.value = eval(gates, g.arg1) << eval(gates, g.arg2);
		}
		else if (g.op == "RSHIFT")
		{
			g.value = eval(gates, g.arg1) >> eval(gates, g.arg2);
		}
		else
		{
			assert(0, "Unknown operand " ~g.op);
		}
		g.computed = true;
	}
	return g.value;
}

static void reset(Gate[string] gates)
{
	foreach (k, g; gates)
	{
		g.computed = false;
	}
}

static void over(Gate[string] gates, string name, uint value)
{
	if (name in gates)
	{
		gates[name].value = value;
		gates[name].computed = true;
	}
}

int main(string[] args)
{
	if (args.length < 2)
	{
		stderr.writefln("Usage: %s <filename>", args[0]);
		return 1;
	}

	Gate[string] gates;
	try
	{
		File input = File(args[1], "rb");
		gates = parse(input);
		input.close();
	}
	catch (std.exception.ErrnoException)
	{
		stderr.writefln("Cannot open %s", args[1]);
		return 1;
	}

	auto part1 = eval(gates, "a");
	writeln("Part1: ", part1);

	reset(gates);
	over(gates, "b", part1);
	writeln("Part2: ", eval(gates, "a"));

	return 0;
}
