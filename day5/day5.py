#!/usr/bin/env python3

import sys

VOWELS = "aeiou"

def is_nice(line):
    vowels = 0
    twice_in_row = 0
    prev = ""
    for x in line:
        if x in VOWELS:
            vowels += 1
        if prev == x:
            twice_in_row += 1
        if prev+x in ("ab", "cd", "pq", "xy"):
            return False
        prev = x

    return vowels >= 3 and twice_in_row >= 1

def is_nice2(line):
    rule1 = False
    rule2 = False

    prev = None
    for i, x in enumerate(line):
        if not rule1:
            if i >= 1 and line[i+1:].find(prev+x) >= 0:
                rule1 = True

        if not rule2:
            if i >= 2 and line[i-2] == x:
                rule2 = True

        if rule1 and rule2:
            break

        prev = x

    return rule1 and rule2

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()
    except OSError:
        print("Cannot open", sys.argv[1], file=sys.stderr)
        sys.exit(1)

    part1 = part2 = 0
    for line in txt.splitlines():
        if is_nice(line):
            part1 += 1
        if is_nice2(line):
            part2 += 1

    print("Part1:", part1)
    print("Part2:", part2)
