#!/usr/bin/env python3

NONE = 0
WIN = 1
LOSE = 2

import re

MISSILE = 0
DRAIN = 1
SHIELD = 2
POISON = 3
RECHARGE = 4

class Game(object):
    SPELLS = [
        ( 53, 0, 4, 0),
        ( 73, 2, 2, 0),
        (113, 0, 0, 6),
        (173, 0, 0, 6),
        (229, 0, 0, 5),
    ]

    def __init__(self, php, pmana, bhp, bdamage, hard = False):
        self.hard = hard
        self.spent_mana = 0
        self.player = php
        self.mana = pmana
        self.armor = 0
        self.boss = bhp
        self.damage = bdamage
        self.effects = [0, 0, 0, 0, 0]

    def apply_effects(self):
        # update the timers
        for i, v in enumerate(self.effects):
            if not v:
                continue

            self.effects[i] = v - 1
            if i == SHIELD and v == 1:
                self.armor -= 7
            elif i == POISON:
                self.boss -= 3
            elif i == RECHARGE:
                self.mana += 101

    def move(self, spell):
        if self.hard:
            self.player -= 1
            if self.player <= 0:
                return LOSE

        self.apply_effects()
        if self.boss <= 0:
            return WIN

        cost, hitpoints, damage, effect = self.SPELLS[spell]
        assert cost <= self.mana
        assert self.effects[spell] == 0

        # cast the spell
        self.spent_mana += cost
        self.mana -= cost
        self.player += hitpoints
        self.boss -= damage
        self.effects[spell] = effect
        if spell == SHIELD:
            self.armor += 7

        if self.boss <= 0:
            return WIN

        # boss turn
        self.apply_effects()
        if self.boss <= 0:
            return WIN

        self.player -= max(1, self.damage - self.armor)
        if self.player <= 0:
            return LOSE

        return NONE

    def permitted_moves(self):
        moves = []
        for i,v in enumerate(self.SPELLS):
            if v[0] <= self.mana and self.effects[i] <= 1:
                moves.append(i)

        return moves

with open("input", "rt") as f:
    txt = f.read()

pattern = re.compile(r"Hit Points: (\d+)|Damage: (\d+)")
for row in txt.split("\n"):
    m = pattern.search(row)
    if not m:
        continue
    hp, damage = m.groups()
    if hp:
        hitpoints = int(hp)
    if damage:
        attack = int(damage)

g = Game(10,250,13,8)
g.move(POISON)
assert g.move(MISSILE) == WIN

g = Game(10, 250, 14, 8)
g.move(RECHARGE)
g.move(SHIELD)
g.move(DRAIN)
g.move(POISON)
assert g.move(MISSILE) == WIN

def run_game(moves, hp, attack, hard):
    g = Game(50, 500, hp, attack, hard)
    # g = Game(10,250,13,8)
    r = NONE
    for m in moves:
        r = g.move(m)
        if r != NONE:
            break

    return g, r

def backtrack(a, hp, attack, hard):
    global best
    g, r = run_game(a, hp, attack, hard)
    if r == WIN and best > g.spent_mana:
        best = g.spent_mana
    elif r == NONE and g.spent_mana < best:
        for m in g.permitted_moves():
            backtrack(a + [m], hp, attack, hard)

best = 1000000000000
backtrack([], hitpoints, attack, False)
print("part1:", best)
best = 1000000000000
backtrack([], hitpoints, attack, True)
print("part2:", best)
