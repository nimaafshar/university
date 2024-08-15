package davis

import "fmt"

type Variable uint

type VariableType uint8

const (
	VariableTypeLocal  VariableType = 0
	VariableTypeInput  VariableType = 1
	VariableTypeOutput VariableType = 2
)

func (v Variable) String() string {
	switch {
	case v == 1:
		return "Y"
	case v%2 == 0:
		return fmt.Sprintf("X%d", v/2)
	default: // v %2 == 1
		return fmt.Sprintf("Z%d", (v-1)/2)
	}
}

// TypeAndIndex return the type of the variable along with
// the index of variable in its type, starting from zero.
func (v Variable) TypeAndIndex() (VariableType, int) {
	switch {
	case v == 1:
		return VariableTypeOutput, 0
	case v%2 == 0:
		return VariableTypeInput, int(v/2) - 1
	default:
		return VariableTypeLocal, int((v-1)/2) - 1
	}
}
