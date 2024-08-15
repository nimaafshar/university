package davis

import "fmt"

type Label uint

const NoLabel = Label(0)

func (l Label) String() string {
	if l == 0 {
		return ""
	}
	switch (l - 1) % 5 {
	case 0:
		return fmt.Sprintf("A%d", (l/5)+1)
	case 1:
		return fmt.Sprintf("B%d", (l/5)+1)
	case 2:
		return fmt.Sprintf("C%d", (l/5)+1)
	case 3:
		return fmt.Sprintf("D%d", (l/5)+1)
	default:
		return fmt.Sprintf("E%d", (l/5)+1)
	}
}
