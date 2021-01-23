#!/usr/bin/env python3

import hashlib
import sys

def find_number(salt, prefix):
    i = 0
    while True:
        generator = "{}{}".format(salt, i)
        hash = hashlib.md5(generator.encode("ascii")).hexdigest()
        if hash.startswith(prefix):
            return i

        i += 1

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read().strip()
    except:
        print("Cannot open", sys.argv[1], file=sys.stderr)
        sys.exit(1)

    print("Part1:", find_number(txt, "00000"))
    print("Part2:", find_number(txt, "000000"))
