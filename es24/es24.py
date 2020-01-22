#!/usr/bin/env python3

from functools import reduce
from operator import mul
from itertools import combinations

def get_entanglement(weights, n):
    group = sum(weights) // n
    for i in range(1, len(weights)+1):
        ent = []
        for c in combinations(weights, i):
            if sum(c) == group:
                ent.append(reduce(mul, c))
        if ent:
            return min(ent)

    return 0

assert get_entanglement([1,2,3,4,5,7,8,9,10,11], 3) == 99
assert get_entanglement([1,2,3,4,5,7,8,9,10,11], 4) == 44

with open("input", "rt") as f:
    weights = [int(x) for x in f.read().strip().split("\n")]

print("part1:", get_entanglement(weights, 3))
print("part2:", get_entanglement(weights, 4))
