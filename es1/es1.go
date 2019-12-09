package main

import (
	"fmt"
	"io/ioutil"
	"os"
)

func main() {
	if len(os.Args) < 2 {
		fmt.Fprintf(os.Stderr, "Usage: %s filename\n", os.Args[0])
		os.Exit(-1)
	}

	values, err := ioutil.ReadFile(os.Args[1])
	if err != nil {
		fmt.Fprintf(os.Stderr, "Cannot open %s for reading\n", os.Args[1])
		os.Exit(-1)
	}

	floor := 0
	pos := 0
	found := false
	for _, v := range values {
		if v == '(' {
			floor++
		} else if v == ')' {
			floor--
		}

		if !found {
			pos++
		}

		if floor == -1 {
			found = true
		}
	}

	fmt.Println("Final floor:", floor)
	fmt.Println("Position of the first char to enter -1:", pos)
}
