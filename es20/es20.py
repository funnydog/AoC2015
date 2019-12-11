#!/usr/bin/env python3

from collections import defaultdict
import math

def find_house(n):
    length = n // 10
    table = [0 for x in range(length+1)]
    for i in range(1, length+1):
        for j in range(i, length+1, i):
            table[j] += i * 10

    for i,v in enumerate(table):
        if v >= n:
            return i

    return -1

def find_house2(n):
    length = n // 11
    table = [0 for x in range(length+1)]
    for i in range(1, length+1):
        for j in range(1,51):
            if i*j < length+1:
                table[i * j] += i * 11

    for i,v in enumerate(table):
        if v >= n:
            return i

    return -1


assert find_house(10) == 1
assert find_house(70) == 4
print("part1:", find_house(36000000))

assert find_house2(11) == 1
assert find_house2(77) == 4
print("part2:", find_house2(36000000))
