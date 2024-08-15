package davis

import (
	. "github.com/onsi/gomega"
	"testing"
)

func TestInstruction_String(t *testing.T) {
	type testCase struct {
		code uint
		ins  string
	}
	testCases := []testCase{
		{
			code: 10,
			ins:  "X1 <- X1 + 1",
		},
		{
			code: 21,
			ins:  "[A1] X1 <- X1 + 1",
		},
		{
			code: 45,
			ins:  "[A1] X1 <- X1 - 1",
		},
		{
			code: 34,
			ins:  "Z2 <- Z2 + 1",
		},
		{
			code: 350,
			ins:  "IF X3 != 0 GOTO B1",
		},
		{
			code: 2,
			ins:  "Y <- Y + 1",
		},
		{
			code: 46,
			ins:  "IF X1 != 0 GOTO A1",
		},
	}

	g := NewWithT(t)
	for _, tc := range testCases {
		ins := DecodeInstruction(tc.code)
		g.Expect(ins.String()).To(Equal(tc.ins), tc.code)
	}

}

func TestOpenTuple(t *testing.T) {
	type testCase struct {
		value uint
		x     uint
		y     uint
	}
	g := NewWithT(t)

	testCases := []testCase{
		{0, 0, 0},
		{21, 1, 5},
	}
	for _, tc := range testCases {
		x, y := DecodeTuple(tc.value)
		g.Expect(x).To(Equal(tc.x), "x")
		g.Expect(y).To(Equal(tc.y), "y")
	}
}
