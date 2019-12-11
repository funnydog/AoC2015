#!/usr/bin/env python3

from itertools import combinations

class Character(object):
    def __init__(self, hitpoints, attack, defense):
        self.hitpoints = hitpoints
        self.defpoints = defense
        self.attackpoints = attack

    def attack(self, other):
        damage = max(1, self.attackpoints - other.defpoints)
        other.hitpoints -= damage

def player_wins(player, boss):
    while True:
        player.attack(boss)
        if boss.hitpoints <= 0:
            return True

        boss.attack(player)
        if player.hitpoints <= 0:
            return False

player = Character(100, 0, 0)
boss = Character(103, 9, 2)

weapons = (
    (8, 4, 0),
    (10, 5, 0),
    (25, 6, 0),
    (40, 7, 0),
    (74, 8, 0),
)

armor = {
    (0, 0, 0),
    (13, 0, 1),
    (31, 0, 2),
    (53, 0, 3),
    (75, 0, 4),
    (102, 0, 5),
}

rings = (
    (0, 0, 0),
    (25, 1, 0),
    (50, 2, 0),
    (100, 3, 0),
    (20, 0, 1),
    (40, 0, 2),
    (80, 0, 3),
)

rings = [(a[0]+b[0], a[1]+b[1], a[2]+b[2]) for a, b in combinations(rings, 2)]
rings.append((0,0,0))
print(rings)
mingold = 1000000000
maxgold = 0
for wc, wd, wa in weapons:
    for ac, ad, aa in armor:
        for rc, rd, ra in rings:
            player = Character(100, wd+ad+rd, wa+aa+ra)
            boss = Character(103, 9, 2)
            if player_wins(player, boss):
                if mingold > wc+ac+rc:
                    mingold = wc+ac+rc
            else:
                if maxgold < wc+ac+rc:
                    maxgold = wc+ac+rc

print("part1", mingold)
print("part2", maxgold)
