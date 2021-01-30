#!/usr/bin/env python3

import re
import sys

def parse(txt):
    pattern = re.compile(r"\w+ (-?\d+),?")
    lst = pattern.findall(txt)
    ing = []
    while lst:
        ing.append(tuple(map(int, lst[:5])))
        lst = lst[5:]
    return ing

def score(ingredients, sol):
    score = 1
    for i in range(len(ingredients[0])-1):
        p = 0
        for j, ing in enumerate(ingredients):
            p += ing[i] * sol[j]
        if p <= 0:
            return 0

        score *= p

    return score

def calories(ingredients, sol):
    return sum(ing[-1] * sol[i] for i, ing in enumerate(ingredients))

def solutions(ing):
    count = len(ing)
    sol = [0] * count
    psum = 0
    while True:
        right = count - 1
        while True:
            right -= 1
            sol[right] += 1
            psum += 1
            if psum <= 100 or right == 0:
                break
            psum -= sol[right]
            sol[right] = 0

        if sol[0] > 100:
            break
        sol[count-1] = 100 - psum
        yield sol

def solve(txt):
    part1, part2 = 0, 0
    ing = parse(txt)
    for x in solutions(ing):
        t = score(ing, x)
        if part1 < t:
            part1 = t
        if calories(ing, x) == 500 and part2 < t:
            part2 = t

    return part1, part2

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

    part1, part2 = solve(txt)
    print("Part1:", part1)
    print("Part2:", part2)
