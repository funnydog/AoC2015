import std.stdio;

int main(string[] args) {
	if (args.length < 2) {
		stderr.writef("Usage: %s filename\n", args[0]);
		return -1;
	}

	File input;
	try {
		input = File(args[1], "r");

		int l, w, h;
		int paper = 0;
		int ribbon = 0;
		while (input.readf(" %sx%sx%s", &l, &w, &h) == 3) {
			int s1 = l * w;
			int s2 = w * h;
			int s3 = h * l;
			paper += 2*(s1+s2+s3);

			if (s2 < s1)
				s1 = s2;
			if (s3 < s1)
				s1 = s3;
			paper += s1;

			int p1 = l + w;
			int p2 = w + h;
			int p3 = h + l;
			if (p2 < p1)
				p1 = p2;
			if (p3 < p1)
				p1 = p3;
			ribbon += 2 * p1;
			ribbon += l * w * h;
		}

		writeln("Total square feet of paper: ", paper);
		writeln("Total feet of ribbon: ", ribbon);
	} catch (std.exception.ErrnoException) {
		stderr.writef("Cannot open %s for reading\n", args[1]);
		return -1;
	}
	input.close();
	return 0;
}
