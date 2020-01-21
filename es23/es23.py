#!/usr/bin/env python3

import re

OP_HLF = 0
OP_TPL = 1
OP_INC = 2
OP_JMP = 3
OP_JIE = 4
OP_JIO = 5

opcodes = {
    "hlf": OP_HLF,
    "tpl": OP_TPL,
    "inc": OP_INC,
    "jmp": OP_JMP,
    "jie": OP_JIE,
    "jio": OP_JIO,
}

pat = re.compile(r"(hlf|tpl|inc) (a|b)" +
                 r"|(jmp) ([+-][0-9]+)" +
                 r"|(jie|jio) (a|b), ([+-][0-9]+)")

def parse(txt):
    lst = []
    for row in txt.split("\n"):
        m = pat.search(row)
        if m is None:
            continue

        op0, arg0, op1, arg1, op2, arg2, arg3 = m.groups()
        if op0:
            lst.append((opcodes[op0], ord(arg0) - ord('a'), None))
        elif op1:
            lst.append((opcodes[op1], int(arg1), None))
        elif op2:
            lst.append((opcodes[op2], ord(arg2) - ord('a'), int(arg3)))
        else:
            print("skipped:", row)

    return lst

def execute(program, initial = 0):
    pc = 0
    reg = [initial, 0]
    while 0 <= pc < len(program):
        op, a, b = program[pc]
        if op == OP_HLF:
            reg[a] //= 2
            pc += 1
        elif op == OP_TPL:
            reg[a] *= 3
            pc += 1
        elif op == OP_INC:
            reg[a] += 1
            pc += 1
        elif op == OP_JMP:
            pc += a
        elif op == OP_JIE:
            if reg[a] % 2 == 0:
                pc += b
            else:
                pc += 1
        elif op == OP_JIO:
            if reg[a] == 1:
                pc += b
            else:
                pc += 1
        else:
            print("UNKNOWN OP")
    return pc, reg[0], reg[1]

txt = """inc a
jio a, +2
tpl a
inc a"""

pc, a, b = execute(parse(txt))
assert(a == 2)

with open("input", "rt") as f:
    txt = f.read()

pc, a, b = execute(parse(txt))
print("part1:", b)
pc, a, b = execute(parse(txt), 1)
print("part2:", b)
