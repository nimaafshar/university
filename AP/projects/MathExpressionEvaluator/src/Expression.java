import java.math.BigDecimal;
import java.util.*;
import java.util.regex.Pattern;


public class Expression {
    private String string;
    Tokenizer tokenizer;
    public ArrayList<Object> tokens;

    static Map<Operator, Integer> precedence;
    static Map<Operator, Associativity> associativity;

    static {
        precedence = new HashMap<>();
        precedence.put(Operator.POW, 4);
        precedence.put(Operator.MULTIPLE, 3);
        precedence.put(Operator.DIVIDE, 3);
        precedence.put(Operator.ADD, 2);
        precedence.put(Operator.SUBTRACT, 2);

        associativity = new HashMap<>();
        associativity.put(Operator.POW, Associativity.RIGHT);
        associativity.put(Operator.MULTIPLE, Associativity.LEFT);
        associativity.put(Operator.DIVIDE, Associativity.LEFT);
        associativity.put(Operator.SUBTRACT, Associativity.LEFT);
        associativity.put(Operator.ADD, Associativity.LEFT);
    }

    public Expression(String s) {
        this.string = s;
        this.tokenizer = new Tokenizer(s);

    }

    public boolean validate() throws TokenizeException, ExpressionFormatException {
        tokens = tokenizer.tokenize();
        Stack<Operator> parenthesisStack = new Stack<>();
        for (int i = 0; i < tokens.size(); i++) {
            if (tokens.get(i) instanceof Operator) {
                if (i != 0 && tokens.get(i - 1) instanceof Operator) {
                    Operator pre = (Operator) tokens.get(i - 1);
                    Operator now = (Operator) tokens.get(i);
                    if ((pre != Operator.LEFT_PARENTHESES && pre != Operator.RIGHT_PARENTHESES && now != Operator.LEFT_PARENTHESES && now != Operator.RIGHT_PARENTHESES)
                            || (pre == Operator.LEFT_PARENTHESES && now == Operator.RIGHT_PARENTHESES))
                        throw new ExpressionFormatException("neighboring Operators");
                }
                Operator token = (Operator) tokens.get(i);

                switch ((Operator) token) {
                    case LEFT_PARENTHESES:
                        parenthesisStack.push((Operator) token);
                        break;
                    case RIGHT_PARENTHESES:
                        if (parenthesisStack.empty()) throw new ExpressionFormatException("Parenthesis error");
                        parenthesisStack.pop();
                        break;
                }
            } else {
                assert tokens.get(i) instanceof BigDecimal : "Nor Operator nor Operand";
            }
        }
        if (!parenthesisStack.empty()) throw new ExpressionFormatException("Parenthesis eror");
        return true;
    }

    public BigDecimal evaluate() throws TokenizeException, ExpressionFormatException {
        this.validate();
        return calculateRPN(this.shuntingYard());
    }


    public Queue<Object> shuntingYard() throws ExpressionFormatException {
        Iterator<Object> it = this.tokens.iterator();
        Queue<Object> output = new LinkedList<>();
        Stack<Operator> operatorStack = new Stack<>();
        while (it.hasNext()) {
            Object rawToken = it.next();
            if (rawToken instanceof BigDecimal) {
                output.add(rawToken);
            } else {
                assert rawToken instanceof Operator;
                Operator token = (Operator) rawToken;
                if (token == Operator.LEFT_PARENTHESES) {
                    operatorStack.push(token);
                } else if (token == Operator.RIGHT_PARENTHESES) {
                    while (!operatorStack.empty() && operatorStack.peek() != Operator.LEFT_PARENTHESES) {
                        output.add(operatorStack.pop());
                    }
                    if (operatorStack.empty()) throw new ExpressionFormatException("Parenthesis error");
                    else operatorStack.pop();
                } else {
                    //real operators
                    while ((!operatorStack.empty()) && (!(operatorStack.peek().equals(Operator.LEFT_PARENTHESES)) && (
                            (precedence.get(operatorStack.peek()) > precedence.get(token)) || (precedence.get(operatorStack.peek()).equals(precedence.get(token)) && associativity.get(operatorStack.peek()) == Associativity.LEFT)
                    ))) {
                        output.add(operatorStack.pop());
                    }
                    operatorStack.push(token);
                }

            }
        }
        while (!operatorStack.empty()) {
            output.add(operatorStack.pop());
        }
        return output;
    }

    public BigDecimal calculateRPN(Queue<Object> postfixExpression) throws ExpressionFormatException {
        Stack<BigDecimal> numbersStack = new Stack<>();
        Iterator it = postfixExpression.iterator();
        while (it.hasNext()){
            Object rawToken = it.next();
            if(rawToken instanceof Operator){
                if(numbersStack.size() >=2){
                    Operator token = (Operator)rawToken;
                    BigDecimal operand_2 = numbersStack.pop();
                    BigDecimal operand_1 = numbersStack.pop();
                    switch (token){
                        case ADD:
                            numbersStack.push(operand_1.add(operand_2));
                            break;
                        case SUBTRACT:
                            numbersStack.push(operand_1.subtract(operand_2));
                            break;
                        case MULTIPLE:
                            numbersStack.push(operand_1.multiply(operand_2));
                            break;
                        case DIVIDE:
                            numbersStack.push(operand_1.divide(operand_2));
                            break;
                        case POW:
                            numbersStack.push(operand_1.pow(operand_2.intValue()));
                            break;
                    }
                }else {
                    throw new ExpressionFormatException();
                }
            }else{
                assert rawToken instanceof BigDecimal;
                numbersStack.push((BigDecimal)rawToken);
            }
        }
        return numbersStack.pop();
    }


}
