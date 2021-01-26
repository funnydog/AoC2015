#!/usr/bin/env python3

import sys

def is_good(pwd):
    rule1 = 0
    rule3 = 0
    last = 0
    pos = 0
    for i, x in enumerate(pwd):
        # rule2: iol forbidden
        if x in "iol":
            return False

        # rule1: three consecutive increasing letters
        if rule1 < 2:
            if last and ord(x) == ord(last)+1:
                rule1 += 1
            else:
                rule1 = 0

        # rule3: non-overlapping pairs of letters
        if last == x and i - pos < 2:
            rule3 += 1
        else:
            pos = i

        last = x

    return rule1 >= 2 and rule3 >= 2

def next_pwd(pwd):
    pwd = list(pwd)
    i = l = len(pwd)
    while i > 0:
        i -= 1
        if pwd[i] != "z":
            pwd[i] = chr(ord(pwd[i])+1)
            if is_good(pwd):
                break

            i = l
        else:
            pwd[i] = "a"

    return "".join(pwd)

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

    pwd = next_pwd(txt)
    print("Part1:", pwd)

    pwd = next_pwd(pwd)
    print("Part2:", pwd)
