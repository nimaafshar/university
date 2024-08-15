package davis

import "fmt"

type InstructionType uint

const (
	// InstructionTypeNoOp corresponds to V <- V instructions
	InstructionTypeNoOp InstructionType = iota
	// InstructionTypeIncr corresponds to V <- V + 1
	InstructionTypeIncr
	// InstructionTypeDecr corresponds to V <- V - 1
	InstructionTypeDecr
	// the other instruction type ( IF V != 0 GO TO L' )
	// doesn't have a specific number
	// it's defined as #(L') + 2
)

type Instruction struct {
	Label Label
	Var   Variable
	Type  InstructionType
}

func (i Instruction) String() string {
	varString := i.Var.String()
	var body string
	switch i.Type {
	case InstructionTypeNoOp:
		body = fmt.Sprintf("%s <- %s", varString, varString)
	case InstructionTypeDecr:
		body = fmt.Sprintf("%s <- %s - 1", varString, varString)
	case InstructionTypeIncr:
		body = fmt.Sprintf("%s <- %s + 1", varString, varString)
	default:
		destLabelString := Label(i.Type - 2).String()
		body = fmt.Sprintf("IF %s != 0 GOTO %s", varString, destLabelString)
	}
	labelString := i.Label.String()
	if labelString != "" {
		return fmt.Sprintf("[%s] %s", labelString, body)
	}
	return body
}

// DecodeInstruction parses the number of instructions of the form <a,<b,c>> to Instruction
func DecodeInstruction(num uint) Instruction {
	a, bc := DecodeTuple(num)
	b, c := DecodeTuple(bc)

	return Instruction{
		Label: Label(a),
		Var:   Variable(c + 1),
		Type:  InstructionType(b),
	}
}

// DecodeTuple converts extracts x,y from a tuple of the form <x,y> = (2^x(2y+1)) - 1
func DecodeTuple(num uint) (x uint, y uint) {
	num = num + 1
	for num%2 == 0 {
		x += 1
		num = num / 2
	}
	y = (num - 1) / 2
	return x, y
}
