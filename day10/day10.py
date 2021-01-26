#!/usr/bin/env python3

import sys

def after(txt, length):
    for _ in range(length):
        other = ""
        count = 0
        a = txt[0]
        for b in txt:
            if a == b:
                count += 1
            else:
                other += str(count) + a
                count = 1
            a = b
        txt = other + str(count) + a

    return txt

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read().strip()
    except OSError as e:
        print(e, file=sys.stderr)
        sys.exit(1)

    txt = after(txt, 40)
    print("Part1:", len(txt))
    txt = after(txt, 10);
    print("Part2:", len(txt))
