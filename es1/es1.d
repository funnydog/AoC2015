import std.stdio;

int main(string[] args) {
	if (args.length < 2) {
		stderr.writef("Usage: %s filename\n", args[0]);
		return -1;
	}

	File input;
	try {
		input = File(args[1], "r");

		int floor = 0;
		size_t pos = 0;
		char c;
		while (input.readf("%c", &c)) {
			if (c == '(') ++floor;
			if (c == ')') --floor;

			if (floor == -1 && pos == 0) {
				pos = input.tell();
			}
		}
		writeln("Final floor: ", floor);
		writeln("Position of the first char to enter -1: ", pos);
	} catch (std.exception.ErrnoException) {
		stderr.writef("Cannot open %s for reading\n", args[1]);
		return -1;
	} finally {
		input.close();
	}

	return 0;
}
