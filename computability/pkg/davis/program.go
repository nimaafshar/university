package davis

import (
	"fmt"
	"strconv"
	"strings"
)

func NewProgram(instructions []Instruction, inputs []int) Program {
	labels := make(map[Label]int)

	maxLocalVarIndex := -1
	maxInputVarIndex := -1
	for index, instruction := range instructions {
		vType, varIdx := instruction.Var.TypeAndIndex()
		switch vType {
		case VariableTypeInput:
			maxInputVarIndex = max(maxInputVarIndex, varIdx)
		case VariableTypeLocal:
			maxLocalVarIndex = max(maxLocalVarIndex, varIdx)
		}

		if instruction.Label == NoLabel {
			continue
		}

		if _, ok := labels[instruction.Label]; !ok {
			labels[instruction.Label] = index
		}
	}

	usedInputVars := make([]int, max(len(inputs), maxInputVarIndex+1))
	for inputIndex, value := range inputs {
		usedInputVars[inputIndex] = value
	}

	return Program{
		instructions:            instructions,
		currentInstructionIndex: 0,
		labels:                  labels,
		output:                  0,
		localVariables:          make([]int, maxLocalVarIndex+1),
		inputs:                  usedInputVars,
		maxUsedInputVarsIndex:   maxInputVarIndex,
	}
}

type Program struct {
	// instructions of the program
	instructions []Instruction
	// currentInstructionIndex is the index of the next instruction to be executed.
	// if its value is more than len(instructions), this mean that the program is terminated.
	currentInstructionIndex int
	// labels contains a map from Label to the number of its instruction
	// the goal of saving this map is faster execution time.
	labels map[Label]int

	// output contains the current value of Y
	output int
	// localVariables contains the value of the local variables
	localVariables []int
	// inputs contains the value of the input variables
	inputs []int
	// maxUsedInputVarsIndex is the index (in the above array) of the biggest
	// (with respect to the order of variables) input variable used in the program
	maxUsedInputVarsIndex int
}

func (p *Program) String() string {
	var output strings.Builder
	output.WriteString(fmt.Sprintln("Instructions"))
	for i, instruction := range p.instructions {
		output.WriteString(fmt.Sprintln(i, instruction.String()))
	}
	output.WriteString(fmt.Sprintln("Labels"))
	for label, instructionNumber := range p.labels {
		output.WriteString(fmt.Sprintln(label.String(), "->", instructionNumber))
	}

	output.WriteString(fmt.Sprintln("Input variables"))
	for index, value := range p.inputs {
		output.WriteString(fmt.Sprintln(Variable((index+1)*2).String(), "->", value))
	}

	output.WriteString(fmt.Sprintln("Local variables"))
	for index, value := range p.inputs {
		output.WriteString(fmt.Sprintln(Variable((index+1)*2+1).String(), "->", value))
	}
	return output.String()
}

func (p *Program) Snapshot() string {
	var builder, inputs, localVariables strings.Builder
	builder.WriteString(fmt.Sprintf("%d ", p.currentInstructionIndex+1))
	for _, value := range p.inputs[:p.maxUsedInputVarsIndex+1] {
		inputs.WriteString(fmt.Sprintf("%d ", value))
	}
	for _, value := range p.localVariables {
		localVariables.WriteString(fmt.Sprintf("%d ", value))
	}
	builder.WriteString(inputs.String())
	builder.WriteString(localVariables.String())
	builder.WriteString(strconv.Itoa(p.output))
	return builder.String()
}

// Step runs one step of the program
func (p *Program) Step() (terminated bool) {
	if p.IsTerminated() {
		return true
	}
	currentInstruction := p.instructions[p.currentInstructionIndex]
	switch currentInstruction.Type {
	case InstructionTypeNoOp:
		p.currentInstructionIndex += 1
	case InstructionTypeIncr:
		p.changeVariableValue(currentInstruction.Var, 1)
		p.currentInstructionIndex += 1
	case InstructionTypeDecr:
		p.changeVariableValue(currentInstruction.Var, -1)
		p.currentInstructionIndex += 1
	default: // IF V != 0 GOTO L instruction
		value := p.variableValue(currentInstruction.Var)
		if value == 0 {
			p.currentInstructionIndex += 1
			return p.IsTerminated()
		}
		instructionIndex, ok := p.labels[Label(currentInstruction.Type-2)]
		if !ok {
			// label doesn't exist, terminating
			p.currentInstructionIndex = len(p.instructions)
			return true
		}
		p.currentInstructionIndex = instructionIndex
	}
	return p.IsTerminated()
}

// variableValue returns the variable value from registry arrays
// only if it exists, otherwise it returns 0
func (p *Program) variableValue(v Variable) int {
	vType, idx := v.TypeAndIndex()
	switch vType {
	case VariableTypeOutput:
		return p.output
	case VariableTypeLocal:
		return p.localVariables[idx]
	case VariableTypeInput:
		return p.inputs[idx]
	default: //unsupported value type
		return 0
	}
}

// changeVariableValue changes variable value in registry arrays
// only if it exists, otherwise it's a NO-OP
func (p *Program) changeVariableValue(v Variable, diff int) {
	vType, idx := v.TypeAndIndex()
	switch vType {
	case VariableTypeOutput:
		p.output += diff
	case VariableTypeLocal:
		p.localVariables[idx] += diff
	case VariableTypeInput:
		p.inputs[idx] += diff
	}
}

func (p *Program) IsTerminated() bool {
	return p.currentInstructionIndex >= len(p.instructions)
}
