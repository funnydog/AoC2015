package main

import (
	"fmt"
	"os"
)

type Agent struct {
	x int
	y int
}

func makeAgent(x, y int) Agent {
	return Agent{x, y}
}

type Map struct {
	xmin, ymin, xmax, ymax int
	width, height          int
	houses                 [][]int
}

func (m *Map) moveAgent(a *Agent, c byte) {
	switch c {
	case '^':
		a.y--
		if a.y < m.ymin {
			m.ymin = a.y
		}
	case 'v':
		a.y++
		if a.y > m.ymax {
			m.ymax = a.y
		}
	case '<':
		a.x--
		if a.x < m.xmin {
			m.xmin = a.x
		}
	case '>':
		a.x++
		if a.x > m.xmax {
			m.xmax = a.x
		}
	}
}

func (m *Map) visitHouse(a *Agent) {
	m.houses[a.y-m.ymin][a.x-m.xmin] += 1
}

func (m *Map) allocateHouses() {
	m.width = m.xmax - m.xmin + 1
	m.height = m.ymax - m.ymin + 1
	m.houses = make([][]int, m.height)
	for y := 0; y < m.height; y++ {
		m.houses[y] = make([]int, m.width)
	}
}

func (m *Map) countHouses(presents int) int {
	count := 0
	for y := 0; y < m.height; y++ {
		for x := 0; x < m.width; x++ {
			if m.houses[y][x] >= presents {
				count++
			}
		}
	}
	return count
}

func makeMap() Map {
	return Map{}
}

func main() {
	rv := mainWithInt()
	if rv != 0 {
		os.Exit(rv)
	}
}

func mainWithInt() int {
	if len(os.Args) < 2 {
		fmt.Fprintf(os.Stderr, "Usage: %s <filename>\n", os.Args[0])
		return -1
	}

	input, err := os.Open(os.Args[1])
	if err != nil {
		fmt.Fprintf(os.Stderr, "Cannot open %s for reading\n", os.Args[1])
		return -1
	}
	defer input.Close()

	m1 := makeMap()
	m2 := makeMap()
	santa1 := makeAgent(0, 0)
	santa2 := makeAgent(0, 0)
	robo2 := makeAgent(0, 0)

	for i := 0; ; i++ {
		var c byte
		_, err = fmt.Fscanf(input, "%c", &c)
		if err != nil {
			break
		}
		m1.moveAgent(&santa1, c)
		if i&1 == 0 {
			m2.moveAgent(&santa2, c)
		} else {
			m2.moveAgent(&robo2, c)
		}
	}

	m1.allocateHouses()
	m2.allocateHouses()

	santa1 = makeAgent(0, 0)
	m1.visitHouse(&santa1)

	santa2 = makeAgent(0, 0)
	robo2 = makeAgent(0, 0)
	m2.visitHouse(&santa2)
	m2.visitHouse(&robo2)

	input.Seek(0, 0)
	for i := 0; ; i++ {
		var c byte
		_, err = fmt.Fscanf(input, "%c", &c)
		if err != nil {
			break
		}
		m1.moveAgent(&santa1, c)
		m1.visitHouse(&santa1)
		if i&1 == 0 {
			m2.moveAgent(&santa2, c)
			m2.visitHouse(&santa2)
		} else {
			m2.moveAgent(&robo2, c)
			m2.visitHouse(&robo2)
		}
	}

	fmt.Println("Answer1:", m1.countHouses(1))
	fmt.Println("Answer2:", m2.countHouses(1))

	return 0
}
