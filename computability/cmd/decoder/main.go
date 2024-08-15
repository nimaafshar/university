package main

import (
	"bufio"
	"fmt"
	"github.com/nimaafshar/davis-interpreter.git/pkg/davis"
	"log"
	"os"
	"strconv"
	"strings"
)

func main() {
	var input string
	reader := bufio.NewReader(os.Stdin)
	input, err := reader.ReadString('\n')
	if err != nil {
		log.Fatal(err)
	}

	for _, numStr := range strings.Split(input[:len(input)-1], " ") {
		num, err := strconv.Atoi(numStr)
		if err != nil || num < 0 {
			fmt.Println(fmt.Errorf("failed to convert %q to a non-negative integer", numStr))
			os.Exit(1)
			return
		}
		instruction := davis.DecodeInstruction(uint(num))
		fmt.Println(instruction.String())
	}
}
