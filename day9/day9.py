#!/usr/bin/env python3

from itertools import permutations
import sys

def distance(loc, dist, default):
    d = 0
    for couple in zip(loc, loc[1:]):
        m = dist.get(couple)
        if m is None:
            return default
        d += m
    return d

def shortest(loc, dist):
    return min(distance(i, dist, 1e12) for i in permutations(loc))

def longest(loc, dist):
    return max(distance(i, dist, 0) for i in permutations(loc))

def parse(txt):
    d = {}
    l = set()
    for line in txt.splitlines():
        lst = line.split(" ")
        if len(lst) > 4:
            src = lst[0]
            dst = lst[2]
            dist = int(lst[4])
            d[src, dst] = dist
            d[dst, src] = dist
            l.add(src)
            l.add(dst)
    return list(l), d

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()
    except OSError as e:
        print(e, file=sys.stderr)
        sys.exit(1)

    loc, dist = parse(txt)
    print("Part1:", shortest(loc, dist))
    print("Part2:", longest(loc, dist))
