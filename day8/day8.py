#!/usr/bin/env python3

import sys

def dec_diff(line):
    real = 0
    state = 0
    hexlen = 0
    for x in line:
        if state == 0:
            if x == "\\":
                state = 1
            elif x != "\"":
                real += 1
        elif state == 1:
            if x == "x":
                state = 2
                hexlen = 2
            elif x in "\\\"":
                real += 1
                state = 0
            else:
                state = 0
        elif state == 2:
            if x in "0123456789abcdef" and hexlen != 1:
                hexlen -= 1
            elif hexlen == 1:
                real += 1
                state = 0
            else:
                state = 0
        else:
            assert 0, "Unknown state {}".format(state)

    return len(line) - real

def enc_diff(line):
    reprlen = 0
    for x in line:
        if x in "\\\"":
            reprlen += 1
        reprlen += 1

    return reprlen + 2 - len(line)

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

    lines = txt.splitlines()
    print("Part1:", sum(map(dec_diff, lines)))
    print("Part2:", sum(map(enc_diff, lines)))
