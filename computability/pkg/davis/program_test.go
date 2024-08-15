package davis

import (
	. "github.com/onsi/gomega"
	"testing"
)

func TestProgram(t *testing.T) {
	t.Run("example 1", func(t *testing.T) {
		g := NewWithT(t)

		instructions := []Instruction{
			DecodeInstruction(45),
			DecodeInstruction(34),
			DecodeInstruction(350),
			DecodeInstruction(2),
			DecodeInstruction(46),
		}
		inputs := []int{2, 1}
		program := NewProgram(instructions, inputs)

		lines := []string{
			"1 2 1 0 0 0 0",
			"2 1 1 0 0 0 0",
			"3 1 1 0 0 1 0",
			"4 1 1 0 0 1 0",
			"5 1 1 0 0 1 1",
			"1 1 1 0 0 1 1",
			"2 0 1 0 0 1 1",
			"3 0 1 0 0 2 1",
			"4 0 1 0 0 2 1",
			"5 0 1 0 0 2 2",
		}
		for _, l := range lines[:len(lines)-1] {
			g.Expect(program.Snapshot()).To(Equal(l))
			g.Expect(program.Step()).To(BeFalse())
		}
		g.Expect(program.Snapshot()).To(Equal(lines[len(lines)-1]))
		g.Expect(program.Step()).To(BeTrue())

		g.Expect(program.Step()).To(BeTrue())
	})
	t.Run("example 2", func(t *testing.T) {
		g := NewWithT(t)

		instructions := []Instruction{
			DecodeInstruction(2),
			DecodeInstruction(6),
		}
		inputs := []int{1}
		program := NewProgram(instructions, inputs)

		lines := []string{
			"1 0",
			"2 1",
		}
		for _, l := range lines[:len(lines)-1] {
			g.Expect(program.Snapshot()).To(Equal(l))
			g.Expect(program.Step()).To(BeFalse())
		}
		g.Expect(program.Snapshot()).To(Equal(lines[len(lines)-1]))
		g.Expect(program.Step()).To(BeTrue())

		g.Expect(program.Step()).To(BeTrue())
	})
}
