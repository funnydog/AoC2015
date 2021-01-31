#!/usr/bin/env python3

import re
import sys

def parse(txt):
    pat = re.compile(r"(\w+):? (\d+),?")
    aunts = []
    aunt = {}
    for key, value in pat.findall(txt):
        if key == "Sue" and aunt:
            aunts.append(aunt)
            aunt = {}
        aunt[key] = int(value)
    aunts.append(aunt)
    return aunts

def remove_if_ne(aunts, prop, value):
    return [a for a in aunts if prop not in a or a[prop] == value]

def remove_if_lte(aunts, prop, value):
    return [a for a in aunts if prop not in a or a[prop] > value]

def remove_if_gte(aunts, prop, value):
    return [a for a in aunts if prop not in a or a[prop] < value]

def solve(txt):
    aunts1 = aunts2 = parse(txt)
    rules = [
        ("children",    3, remove_if_ne),
        ("cats",        7, remove_if_lte),
        ("samoyeds",    2, remove_if_ne),
        ("pomeranians", 3, remove_if_gte),
        ("akitas",      0, remove_if_ne),
        ("vizslas",     0, remove_if_ne),
        ("goldfish",    5, remove_if_gte),
        ("trees",       3, remove_if_lte),
        ("cars",        2, remove_if_ne),
        ("perfumes",    1, remove_if_ne),
    ]
    for prop, value, fn in rules:
        aunts1 = remove_if_ne(aunts1, prop, value)
        aunts2 = fn(aunts2, prop, value)

    return aunts1[0]["Sue"], aunts2[0]["Sue"]

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

    part1, part2 = solve(txt)
    print("Part1:", part1)
    print("Part2:", part2)
