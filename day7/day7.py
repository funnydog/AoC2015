#!/usr/bin/env python3

import sys

class Gate(object):
    def __init__(self, op, arg1, arg2):
        self.op = op
        self.arg1 = arg1
        self.arg2 = arg2
        self.value = None

def parse(txt):
    gates = {}
    for line in txt.splitlines():
        ops, name = line.split(" -> ")
        lst = ops.split(" ")
        if len(lst) == 1:
            op, arg1, arg2 = "ASSIGN", lst[0], None
        elif len(lst) == 2:
            op, arg1, arg2 = lst[0], lst[1], None
        else:
            arg1, op, arg2 = lst
        gates[name] = Gate(op, arg1, arg2)

    return gates

def reset(gates):
    for k, v in gates.items():
        v.value = None

def override(gates, name, value):
    g = gates.get(name)
    if g:
        g.value = value

def evaluate(gates, name):
    if name not in gates:
        return int(name)

    g = gates[name]
    if g.value is None:
        if g.op == "ASSIGN":
            g.value = evaluate(gates, g.arg1)
        elif g.op == "NOT":
            g.value = ~evaluate(gates, g.arg1)
        elif g.op == "AND":
            g.value = evaluate(gates, g.arg1) & evaluate(gates, g.arg2)
        elif g.op == "OR":
            g.value = evaluate(gates, g.arg1) | evaluate(gates, g.arg2)
        elif g.op == "LSHIFT":
            g.value = evaluate(gates, g.arg1) << evaluate(gates, g.arg2)
        elif g.op == "RSHIFT":
            g.value = evaluate(gates, g.arg1) >> evaluate(gates, g.arg2)
        else:
            assert 0, "Unknown op " + g.op

    return g.value

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()
    except OSError:
        print("Cannot open", sys.argv[1], file=sys.stderr)
        sys.exit(1)

    gates = parse(txt)
    part1 = evaluate(gates, "a")
    print("Part1:", part1)

    reset(gates)
    override(gates, "b", part1)
    print("Part2:", evaluate(gates, "a"))
