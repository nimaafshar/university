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
	var instructionsLine, inputsLine string
	reader := bufio.NewReader(os.Stdin)
	instructionsLine, err := reader.ReadString('\n')
	if err != nil {
		log.Fatal(err)
	}
	inputsLine, err = reader.ReadString('\n')
	if err != nil {
		log.Fatal(err)
	}

	instructionsStrings := strings.Split(instructionsLine[:len(instructionsLine)-1], " ")
	instructions := make([]davis.Instruction, len(instructionsStrings))
	for i, instrString := range instructionsStrings {
		num, err := strconv.Atoi(instrString)
		if err != nil || num < 0 {
			fmt.Println(fmt.Errorf("failed to convert %q to a non-negative integer", instrString))
			os.Exit(1)
			return
		}
		instructions[i] = davis.DecodeInstruction(uint(num))
	}
	inputStrings := strings.Split(inputsLine[:len(inputsLine)-1], " ")
	inputs := make([]int, len(inputStrings))
	for i, inputString := range inputStrings {
		inputs[i], err = strconv.Atoi(inputString)
		if err != nil {
			fmt.Println(fmt.Errorf("failed to convert %q to an integer", inputString))
			os.Exit(1)
			return
		}
	}

	program := davis.NewProgram(instructions, inputs)
	if err != nil {
		fmt.Println(fmt.Errorf("invalid program: %w", err))
		os.Exit(1)
		return
	}

	terminated := false
	for !terminated {
		fmt.Println(program.Snapshot())
		terminated = program.Step()
	}
}
