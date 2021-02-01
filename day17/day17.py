#!/usr/bin/env python3

from functools import cache
import sys

@cache
def part1(containers, capacity):
    if capacity == 0:
        return 1
    if capacity < 0 or not containers:
        return 0

    return part1(containers[1:], capacity - containers[0]) + part1(containers[1:], capacity)

@cache
def part2(containers, capacity, count):
    if capacity == 0 and count == 0:
        return 1
    if capacity < 0 or not containers:
        return 0
    return part2(containers[1:], capacity-containers[0], count-1) + part2(containers[1:], capacity, count)

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

    containers = tuple(map(int, txt.split()))
    print("Part1:", part1(containers, 150))
    count = 0
    while True:
        p = part2(containers, 150, count)
        if p != 0:
            break
        count += 1
    print("Part2:", p)
