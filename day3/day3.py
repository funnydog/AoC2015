#!/usr/bin/env python3

import sys

def move(p, d):
    if d == "^":
        return (p[0], p[1]-1)
    elif d == ">":
        return (p[0]+1, p[1])
    elif d == "v":
        return (p[0], p[1]+1)
    elif d == "<":
        return (p[0]-1, p[1])
    else:
        print(d)
        assert 0, "Unknown direction"

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read().strip()
    except:
        print("Cannot open", sys.argv[1], file=sys.stderr)
        sys.exit(1)

    mymap = {}
    santa = (0, 0)
    mymap[santa] = 1
    for d in txt:
        santa = move(santa, d)
        mymap[santa] = mymap.get(santa, 0) + 1

    print("Part1:", len(mymap))

    mymap = {}
    santa, robosanta = (0,0), (0,0)
    mymap[santa] = 2
    for i, d in enumerate(txt):
        if i % 2 == 0:
            cur = santa = move(santa, d)
        else:
            cur = robosanta = move(robosanta, d)

        mymap[cur] = mymap.get(cur, 0) + 1

    print("Part2:", len(mymap))
