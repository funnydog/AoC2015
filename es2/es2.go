package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func Min(values []int) int {
	min := values[0]
	for _, v := range values {
		if v < min {
			min = v
		}
	}
	return min
}

func Max(values []int) int {
	max := values[0]
	for _, v := range values {
		if v > max {
			max = v
		}
	}
	return max
}

func Sum(values []int) int {
	sum := 0
	for _, v := range values {
		sum += v
	}
	return sum
}

func Mul(values []int) int {
	mul := 1
	for _, v := range values {
		mul *= v
	}
	return mul
}

func main() {
	if len(os.Args) < 2 {
		fmt.Fprintf(os.Stderr, "Usage: %s filename\n", os.Args[0])
		os.Exit(-1)
	}

	input, err := os.Open(os.Args[1])
	if err != nil {
		fmt.Fprintf(os.Stderr, "Cannot open %s for reading\n", os.Args[1])
		os.Exit(-1)
	}
	defer input.Close()

	paper := 0
	ribbon := 0
	scanner := bufio.NewScanner(input)
	for scanner.Scan() {
		s := strings.Split(scanner.Text(), "x")
		dim := []int{}
		for _, val := range s {
			i, _ := strconv.ParseInt(val, 10, 32)
			dim = append(dim, int(i))
		}

		sides := []int{}
		perims := []int{}
		for i, val := range dim {
			sides = append(sides, val*dim[(i+1)%3])
			perims = append(perims, 2*(val+dim[(i+1)%3]))
		}

		paper += 2 * Sum(sides)
		paper += Min(sides)

		ribbon += Min(perims)
		ribbon += Mul(dim)
	}

	fmt.Println("Total square feet of paper:", paper)
	fmt.Println("Total feet of ribbon:", ribbon)
}
