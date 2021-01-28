#!/usr/bin/env python3

from itertools import permutations
import re
import sys

def parse(txt):
    pat = re.compile(r"(\w+) would (gain|lose) (\d+) happiness units by sitting next to (\w+).")
    names = {}
    for one, gain, units, other in pat.findall(txt):
        dct = names.get(one, {})
        units = int(units)
        if gain != "gain":
            units = -units
        dct[other] = units
        names[one] = dct

    return names

def happiness(names, solution):
    h = 0
    left = solution[-2]
    cur = solution[-1]
    for right in solution:
        h += names[cur].get(left, 0)
        h += names[cur].get(right, 0)
        left = cur
        cur = right
    return h

def maximize(names):
    solution = list(names.keys())
    best = -1e12
    for lst in permutations(solution):
        h = happiness(names, lst)
        if best < h:
            best = h
    return best

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

    names = parse(txt)
    print("Part1:", maximize(names))
    names["me"] = {}
    print("Part2:", maximize(names))
