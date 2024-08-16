public class ExpressionFormatException extends Exception {
    public ExpressionFormatException(String parenthesis_eror) {
        super(parenthesis_eror);
    }
    public ExpressionFormatException(){
        super();
    }
}
