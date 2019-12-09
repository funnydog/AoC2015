#!/usr/bin/env python3

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2 :
        print("Usage: {} filename".format(sys.argv[0]), file = sys.stderr)
        exit(-1)

    try:
        with open(sys.argv[1], "rt") as f:
            values = list(f.read())
    except:
        print("Cannot open {} for reading".format(sys.argv[1]), file = sys.stderr)
        exit(-1)

    floor = 0
    pos = 0
    found = False
    for v in values:
        if v == "(":
            floor += 1
        elif v == ")":
            floor -= 1

        if not found:
            pos += 1

        if floor == -1:
            found = True

    print("Final floor: {}".format(floor))
    print("Position of the first char to enter -1: {}".format(pos))

