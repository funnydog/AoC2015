#!/usr/bin/env python3

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2 :
        print("Usage: {} filename".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()
    except:
        print("Cannot open", sys.argv[1], file=sys.stderr)
        sys.exit(1)

    floor = 0
    found = None
    for i, v in enumerate(txt):
        if v == "(":
            floor += 1
        elif v == ")":
            floor -= 1

        if found is None and floor == -1:
            found = i + 1

    print("Part1:", floor)
    print("Part2:", found)

