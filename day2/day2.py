#!/usr/bin/env python3

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} filename".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()
    except:
        print("Cannot open", sys.argv[1], file=sys.stderr)
        sys.exit(1)

    paper = 0
    ribbon = 0
    for line in txt.splitlines():
        l, w, h = sorted(map(int, line.split("x")))
        paper += 3 * l * w + 2 * h * (w + l)
        ribbon += 2 * (l+w) + l * w * h;

    print("Part1:", paper)
    print("Part2:", ribbon)
