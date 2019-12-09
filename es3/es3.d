import std.stdio;
import std.format;

struct Agent {
	int x, y;

	this(int x, int y) {
		this.x = x;
		this.y = y;
	}
}

struct Map {
	int xmin, xmax;
	int ymin, ymax;
	int width, height;

	int[][] map;

	void moveAgent(ref Agent a, char c) {
		switch (c) {
		case '^':
			if (--a.y < this.ymin) {
				this.ymin = a.y;
			}
			break;

		case 'v':
			if (++a.y > this.ymax) {
				this.ymax = a.y;
			}
			break;

		case '<':
			if (--a.x < this.xmin) {
				this.xmin= a.x;
			}
			break;

		case '>':
			if (++a.x > this.xmax) {
				this.xmax = a.x;
			}
			break;

		default:
			break;
		}
	}

	void createMap() {
		this.width = this.xmax - this.xmin + 1;
		this.height = this.ymax - this.ymin + 1;
		this.map = new int[][](this.height, this.width);
	}

	void visit(ref Agent a) {
		this.map[a.y - this.ymin][a.x - this.xmin] += 1;
	}

	int countHouses(int presents) {
		int count = 0;
		for (int y = 0; y < this.height; y++) {
			for (int x = 0; x < this.width; x++) {
				if (this.map[y][x] >= presents) {
					count++;
				}
			}
		}
		return count;
	}
}

int main(string[] args) {
	if (args.length < 2) {
		stderr.writef("Usage: %s <filename>\n", args[0]);
		return -1;
	}

	File input;
	try {
		input = File(args[1], "r");
	} catch (std.exception.ErrnoException) {
		stderr.writef("Cannot open %s for reading\n", args[1]);
	}

	Agent santa1 = Agent(0,0);
	Agent santa2 = Agent(0,0);
	Agent robo = Agent(0,0);
	Map m1 = Map();
	Map m2 = Map();
	for (int i = 0; !input.eof(); i++) {
		char c;
		input.readf("%c", &c);

		m1.moveAgent(santa1, c);
		if (i & 1) {
			m2.moveAgent(robo, c);
		} else {
			m2.moveAgent(santa2, c);
		}
	}

	m1.createMap();
	m2.createMap();

	santa1 = Agent(0,0);
	m1.visit(santa1);

	santa2 = Agent(0,0);
	robo = Agent(0,0);
	m2.visit(santa2);
	m2.visit(robo);

	input.seek(0, 0);
	for (int i = 0; !input.eof(); i++) {
		char c;
		input.readf("%c", &c);

		m1.moveAgent(santa1, c);
		m1.visit(santa1);
		if (i & 1) {
			m2.moveAgent(robo, c);
			m2.visit(robo);
		} else {
			m2.moveAgent(santa2, c);
			m2.visit(santa2);
		}
	}
	input.close();

	writeln("Answer1: ", m1.countHouses(1));
	writeln("Answer2: ", m2.countHouses(1));

	return 0;
}
