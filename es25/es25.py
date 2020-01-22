#!/usr/bin/env python3

import re

def value(x, y):
    assert x > 0 and y > 0
    ny = y + x - 1
    return ny * (ny - 1) // 2 + x

def find_code(pos):
    assert pos > 0
    return (20151125 * pow(252533, pos-1, 33554393)) % 33554393

with open("input", "rt") as f:
    txt = f.read()

m = re.search(r"To continue, please consult the code grid in the manual.  Enter the code at row ([0-9]+), column ([0-9]+).", txt)
if m:
    x = int(m.group(2))
    y = int(m.group(1))
    print("part1:", find_code(value(x, y)))
