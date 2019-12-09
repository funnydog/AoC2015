#!/usr/bin/env python3

import sys

class Agent(object):
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def visitHouse(self, map):
        map.houses[self.y - map.ymin][self.x - map.xmin] += 1

class Map(object):
    def __init__(self):
        self.xmin = 0
        self.xmax = 0
        self.ymin = 0
        self.ymax = 0
        self.width = 0
        self.height = 0
        self.houses = None

    def moveAgent(self, agent, b):
        if b == '^':
            agent.y -= 1
            if agent.y < self.ymin:
                self.ymin = agent.y
        elif b == 'v':
            agent.y += 1
            if agent.y > self.ymax:
                self.ymax = agent.y
        elif b == '<':
            agent.x -= 1
            if agent.x < self.xmin:
                self.xmin = agent.x
        elif b == '>':
            agent.x += 1
            if agent.x > self.xmax:
                self.xmax = agent.x

    def createHouses(self):
        self.width = self.xmax - self.xmin + 1
        self.height = self.ymax - self.ymin + 1
        self.houses = [[0 for x in range(self.width)] for y in range(self.height)]

    def countHouses(self, presents):
        count = 0
        for y in range(self.height):
            for x in range(self.width):
                if self.houses[y][x] >= presents:
                    count += 1
        return count

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file = sys.stderr)
        exit(-1)

    line = ""
    try:
        with open(sys.argv[1], "rt") as f:
            line = f.read()
    except:
        print("Cannot open {} for reading".format(sys.argv[1]), file = sys.stderr)
        exit(-1)

    m1 = Map()
    m2 = Map()
    santa1 = Agent(0, 0)
    santa2 = Agent(0, 0)
    robo2 = Agent(0, 0)
    for i in range(len(line)):
        m1.moveAgent(santa1, line[i])
        if i & 1 == 0:
            m2.moveAgent(santa2, line[i])
        else:
            m2.moveAgent(robo2, line[i])

    m1.createHouses()
    santa1 = Agent(0, 0)
    santa1.visitHouse(m1)

    m2.createHouses()
    santa2 = Agent(0, 0)
    santa2.visitHouse(m2)
    robo2 = Agent(0, 0)
    robo2.visitHouse(m2)

    for i in range(len(line)):
        m1.moveAgent(santa1, line[i])
        santa1.visitHouse(m1)
        if i & 1 == 0:
            m2.moveAgent(santa2, line[i])
            santa2.visitHouse(m2)
        else:
            m2.moveAgent(robo2, line[i])
            robo2.visitHouse(m2)

    print("Answer1: {}".format(m1.countHouses(1)))
    print("Answer2: {}".format(m2.countHouses(1)))
