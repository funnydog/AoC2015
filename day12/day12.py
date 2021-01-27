#!/usr/bin/env python3

import re
import sys

def sum_integers(tokens):
    s = 0
    for x in tokens:
        if x[0] in "-0123456789":
            s += int(x)
    return s

pos = 0
def recursive_sum(tokens):
    global pos
    if pos >= len(tokens):
        # EOT
        return 0
    elif tokens[pos][0] in "-0123456789":
        # numbers
        v = int(tokens[pos])
        pos += 1
        return v
    elif tokens[pos][0] == "\"":
        # strings
        pos += 1
        return 0
    elif tokens[pos] == "[":
        # arrays
        v = 0
        while True:
            pos += 1
            v += recursive_sum(tokens)
            if tokens[pos] != ",":
                break
        assert tokens[pos] == "]", "Malformed input"
        pos += 1
        return v
    elif tokens[pos] == "{":
        # dictionaries
        v, ignore = 0, False
        while True:
            pos += 1
            pos += 1            # skip the key
            assert tokens[pos] == ":", "Malformed input"
            pos += 1
            if tokens[pos] == "\"red\"":
                ignore = True
                pos += 1
            else:
                v += recursive_sum(tokens)

            if tokens[pos] != ",":
                break

        assert tokens[pos] == "}", "Malformed input"
        pos += 1

        if ignore:
            return 0
        else:
            return v
    else:
        assert 0, "Unreachable code"

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

    pat = re.compile(r"[{}\[\],:]|\"[^\"]*\"|-?\d+")
    tokens = pat.findall(txt)
    print("Part1:", sum_integers(tokens))
    print("Part2:", recursive_sum(tokens))
