import javax.management.AttributeList;
import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Tokenizer {
    private String expression;
    private static final Pattern pattern = Pattern.compile("(\\b\\w*[\\.]?\\w+\\b|[\\(\\)\\+\\*\\-\\/\\^])");

    public Tokenizer(String expression){
        this.expression = expression;
    }

    public ArrayList<Object> tokenize() throws TokenizeException,NumberFormatException {
        Matcher m = pattern.matcher(this.expression);
        ArrayList<String> tokens = new ArrayList<>();
        while(m.find()){
            int count = m.groupCount();
            for(int i=0;i<count;i++){
                tokens.add(m.group(i));
            }
        }

        if(tokens.isEmpty()){
            throw new TokenizeException("Not Found");
        }else {
            ArrayList<Object> trueTokens = new ArrayList<>();
            for(String token:tokens){
                switch (token){
                    case "+":
                        trueTokens.add(Operator.ADD);
                        break;
                    case "-":
                        trueTokens.add(Operator.SUBTRACT);
                        break;
                    case "*":
                        trueTokens.add(Operator.MULTIPLE);
                        break;
                    case "/":
                        trueTokens.add(Operator.DIVIDE);
                        break;
                    case "^":
                        trueTokens.add(Operator.POW);
                        break;
                    case "(":
                        trueTokens.add(Operator.LEFT_PARENTHESES);
                        break;
                    case ")":
                        trueTokens.add(Operator.RIGHT_PARENTHESES);
                        break;

                        default:
                            BigDecimal bigDecimal = new BigDecimal(token);
                            trueTokens.add(bigDecimal);
                }
            }
            return trueTokens;
        }
    }
}
