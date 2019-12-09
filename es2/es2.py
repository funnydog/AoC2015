#!/usr/bin/env python3

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} filename".format(sys.argv[0]))
        sys.Exit(-1)

    try:
        with open(sys.argv[1], "rt") as file:
            paper = 0
            ribbon = 0
            for line in file:
                dima = [int(x) for x in line.split("x")]
                dimb = dima[1:] + dima[0:1]
                sides = [dima[x] * dimb[x] for x in range(len(dima))]
                paper += 2 * sum(sides) + min(sides)

                perim = [2*(dima[x] + dimb[x]) for x in range(len(dima))]
                ribbon += min(perim) + dima[0]*dima[1]*dima[2]

        print("Total square feet of paper: {}".format(paper))
        print("Total feet of ribbon: {}".format(ribbon))
    except Exception as e:
        print(e)
        print("Cannot open {} for reading".format(sys.argv[1]))
