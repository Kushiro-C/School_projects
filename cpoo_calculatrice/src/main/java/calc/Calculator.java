package calc;

import java.math.BigDecimal;
import java.math.RoundingMode;

/**
 * Class Calculator
 * This class will take care of the arithmetic
 * operations (such as addition, subtraction, etc...)
 * for our RPN calculator.
 * Not mutable class
 * @author Chang Patrick
 * @author Du Vincent
 */
public abstract class Calculator {

    private Calculator() {}

    /**
     * Add x to y
     * 
     * @param x BigDecimal
     * @param y BigDecimal
     * @return BigDecimal (x + y)
     */
    protected static BigDecimal addition(BigDecimal x, BigDecimal y) {
        return x.add(y);
    }

    /**
     * Substract y to x
     * 
     * @param x BigDecimal
     * @param y BigDecimal
     * @return BigDecimal (x - y)
     */
    protected static BigDecimal substraction(BigDecimal x, BigDecimal y) {
        return x.subtract(y);
    }

    /**
     * Multiply x by y
     * 
     * @param x BigDecimal
     * @param y BigDecimal
     * @return BigDecimal (x * y)
     */
    protected static BigDecimal multiplication(BigDecimal x, BigDecimal y) {
        return x.multiply(y);
    }

    /**
     * Divide x by y
     * We allow up to 12 decimal digits
     * (ex : 1/3 = 0.333333333333 with twelve 3)
     * 
     * @param x BigDecimal
     * @param y BigDecimal
     * @return BigDecimal (x / y)
     */
    protected static BigDecimal division(BigDecimal x, BigDecimal y) {
        if (y.doubleValue() == 0) {
            System.out.println("Division by zero");
            throw new IllegalArgumentException("Division by zero");
        }
        return x.divide(y, 12, RoundingMode.HALF_UP);
    }


    /**
     * Calculates the operation
     * between x and y, with the
     * operator op as : x op y
     * Removes the excess decimal zeros
     * 
     * @param x BigDecimal
     * @param y BigDecimal
     * @param op operator ['+', '-', '*', '/']
     * @return the result of the calculation
     * In case the op was not verified, then
     * this function will throw an exception
     */
    protected static BigDecimal calculate(BigDecimal x, BigDecimal y, char op) {
        switch (op) {
            case '+' :
                return addition(x, y).stripTrailingZeros();
            case '-' :
                return substraction(x, y).stripTrailingZeros();
            case '*' :
                return multiplication(x, y).stripTrailingZeros();
            case '/' :
                return division(x, y).stripTrailingZeros();
            default :
                break;
        }

        System.out.println("Operator not valid");
        throw new IllegalArgumentException("Operator not valid");
    }


    /**
     * Add x to y
     * 
     * @param x int
     * @param y int
     * @return int (x + y)
     */
    protected static int addition(int x, int y) {
        return x + y;
    }

    /**
     * Substract y to x
     * 
     * @param x int
     * @param y int
     * @return int (x - y)
     */
    protected static int substraction(int x, int y) {
        return x - y;
    }

    /**
     * Multiply x by y
     * 
     * @param x int
     * @param y int
     * @return int (x * y)
     */
    protected static int multiplication(int x, int y) {
        return x * y;
    }

    /**
     * Euclidean Division of : x by y
     * (2/3 = 1)
     * 
     * @param x int
     * @param y int
     * @return int (x / y)
     */
    protected static int division(int x, int y) {
        if (y == 0) {
            System.out.println("Division by zero");
            throw new IllegalArgumentException("Division by zero");
        }
        return x / y;
    }


    /**
     * Calculates the operation
     * between x and y, with the
     * operator op as : x op y
     * Removes the excess decimal zeros
     * 
     * @param x int
     * @param y int
     * @param op operator ['+', '-', '*', '/']
     * @return the result of the calculation
     * In case the op was not verified, then
     * this function will throw an exception
     */
    protected static int calculate(Integer x, Integer y, char op) {
        switch (op) {
            case '+' :
                return addition(x, y);
            case '-' :
                return substraction(x, y);
            case '*' :
                return multiplication(x, y);
            case '/' :
                return division(x, y);
            default :
                break;
        }

        System.out.println("Operator not valid");
        throw new IllegalArgumentException("Operator not valid");
    }
}
