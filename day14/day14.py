#!/usr/bin/env python3

import re
import sys

def parse(txt):
    pat = re.compile(r"\w+ can fly (\d+) km/s for (\d+) seconds, but then must rest for (\d+) seconds.")
    return [tuple(map(int, x)) for x in pat.findall(txt)]

def distance(reindeer, seconds):
    q, r = divmod(seconds, reindeer[1] + reindeer[2])
    if r > reindeer[1]:
        r = reindeer[1]

    return reindeer[0] * (q * reindeer[1] + r)

def points(reindeers, seconds):
    points = [0] * len(reindeers)
    dist = [0] * len(reindeers)
    for t in range(1, seconds+1):
        maxd = 0
        for i, r in enumerate(reindeers):
            dist[i] = distance(r, t)
            if maxd < dist[i]:
                maxd = dist[i]

        for i, d in enumerate(dist):
            if d == maxd:
                points[i] += 1

    return points

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

    reindeers = parse(txt)
    print("Part1:", max(distance(r, 2503) for r in reindeers))
    print("Part2:", max(points(reindeers, 2503)))
