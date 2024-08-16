import java.util.ArrayList;
import java.util.Scanner;

public class Main {

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        String a = scanner.nextLine();
        Expression expression = new Expression(a);
        try {
            System.out.println(expression.evaluate());
        } catch (TokenizeException | ExpressionFormatException | NumberFormatException e) {
            System.out.println("Error: "+e.getMessage());
        }
    }
}
//((15 ÷ (7 − (1 + 1))) × 3) − (2 + (1 + 1))
//((15 / (7 - (1 + 1))) * 3) - (2 + (1 + 1))
