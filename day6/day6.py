#!/usr/bin/env python3

import re
import sys

pat = re.compile(r"(turn on|toggle|turn off) (\d+),(\d+) through (\d+),(\d+)")

def part1(txt):
    table = [[0] * 1000 for y in range(1000)]
    for action, a, b, c, d in pat.findall(txt):
        x1, y1, x2, y2 = map(int, (a,b,c,d))
        for y in range(y1, y2+1):
            for x in range(x1, x2+1):
                if action == "turn on":
                    table[y][x] = 1
                elif action == "toggle":
                    table[y][x] = 1 - table[y][x]
                else:
                    table[y][x] = 0

    return sum(sum(row) for row in table)

def part2(txt):
    table = [[0] * 1000 for y in range(1000)]
    for action, a, b, c, d in pat.findall(txt):
        x1, y1, x2, y2 = map(int, (a,b,c,d))
        for y in range(y1, y2+1):
            for x in range(x1, x2+1):
                if action == "turn on":
                    table[y][x] += 1
                elif action == "toggle":
                    table[y][x] += 2
                elif table[y][x] > 0:
                    table[y][x] -= 1

    return sum(sum(row) for row in table)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()
    except:
        print("Cannot open", sys.argv[1], file=sys.stderr)
        sys.exit(1)

    print("Part1:", part1(txt))
    print("Part2:", part2(txt))
