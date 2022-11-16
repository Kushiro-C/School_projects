package calc;

import java.util.regex.Pattern;

/**
 * Class MyParser
 * This class contains functions for parsing
 * a line from the user, and returning
 * informations needed for our RPN calculator
 * Not mutable class
 * @author Chang Patrick
 * @author Du Vincent
 */
public final class MyParser {

    private MyParser() {}

    /**
     * Checks if the operator is valid,
     * it must be in this selection :
     * ['+', '-', '*', '/']
     * 
     * @param op
     * @return true if the operator is valid
     */
    protected static boolean validOperator(char op) {
        return (op == '+' || op == '-' || op == '*' || op == '/');
    }

    /**
     * Checks if the operator is valid,
     * it must be in this selection :
     * ["ET", "OU", "NON"]
     * 
     * @param op
     * @return true if the operator is valid
     */
    protected static boolean validOperatorBoolean(String op) {
        return (op.toUpperCase().equals("ET")
            || op.toUpperCase().equals("OU")
            || op.toUpperCase().equals("NON"));
    }

    /**
     * Checks if the string is a valid operator,
     * in other terms, can be converted as a char
     * 
     * @param op String
     * @return true if the string represents an operator
     */
    protected static boolean isOperator(String op) {
        if (op.length() == 1)
            return validOperator(op.charAt(0));

        return false;
    }

    /**
     * Checks if the String is convertable
     * into a double
     * 
     * @param num
     * @return true if String can be converted
     * as a double, false otherwise
     */
    protected static boolean isDouble(String num) {
        if (num == null || num.length() < 1)
            return false;

        String decimalPattern = "(-?)([0-9]+)\\.?([0-9]*)";
        return Pattern.matches(decimalPattern, num);
    }

    /**
     * Checks if the String is convertable
     * into an int
     * 
     * @param num
     * @return true if String can be converted
     * as an int, false otherwise
     */
    protected static boolean isInt(String num) {
        if (num == null || num.length() < 1)
            return false;

        String integerPattern = "(-?)([0-9]+)";
        return Pattern.matches(integerPattern, num);
    }

    /**
     * Checks if the String is convertable
     * into an boolean
     * 
     * @param s
     * @return true if String can be converted
     * as an boolean, false otherwise
     */
    protected static boolean isBoolean(String s) {
        return (s.toUpperCase().equals("VRAI")
            || s.toUpperCase().equals("FAUX"));
    }

    /**
     * get the boolean variable, supposed that we used isBool before
     * @param s the string
     * @return boolean,
     * null otherwise
     */
    protected static boolean getBoolean(String s) {
        if (s.toUpperCase().equals("VRAI"))
            return true;
        else
            return false;
    }

    /**
     * Checks the String is hist command
     * 
     * @param s the string
     * @return true if String as "hist(2)",
     * false otherwise
     */
    protected static boolean isHist(String s) {
        if (s == null || s.length() < 7)
            return false;
        if (s.substring(0, 5).equals("hist(")) {
            if (s.length() == 7) {
                if (isInt(Character.toString(s.charAt(5))))
                    if (s.endsWith(")"))
                        return true;
            }
            else if (isInt(s.substring(5, s.length()-1))) {
                if (s.endsWith(")"))
                    return true;
            }
        }
        return false;
    }

    /**
     * Checks the String is pile command
     * @param s the string
     * @return true if String as "pile(2)",
     * false otherwise
     */
    protected static boolean isPile(String s) {
        if (s == null || s.length() < 7)
            return false;
        if (s.substring(0, 5).equals("pile(")) {
            if (s.length() == 7) {
                if (isInt(Character.toString(s.charAt(5))))
                    if (s.endsWith(")"))
                        return true;
            }
            else if (isInt(s.substring(5, s.length()-1))) {
                if (s.endsWith(")"))
                    return true;
            }
        }
        return false;
    }

    /**
     * get the numeric variable of hist or pile command
     * @param s the string
     * @return substring of the numeric variable,
     * null otherwise
     */
    protected static String getHistOrPile(String s) {
        if (isHist(s) || isPile(s)) {
            if (s.length() == 7)
                return Character.toString(s.charAt(5));
            else
                return s.substring(5, s.length()-1);
        }
        return null;
    }

    /**
     * Checks if the String is pop a variable
     * @param s the string
     * @return true if String as "!a",
     * false otherwise
     */
    protected static boolean isPopVar(String s) {
        if (s == null || s.length() < 2)
            return false;
        if (s.startsWith("!")) {
            if (s.substring(1).matches("[a-zA-Z]+"))
                return true;
        }
        return false;
    }

    /**
     * Checks if the String is stack a variable
     * @param s the string
     * @return true if String as "?a",
     * false otherwise
     */
    protected static boolean isStackVar(String s) {
        if (s == null || s.length() < 2)
            return false;
        if (s.startsWith("?")) {
            if (s.substring(1).matches("[a-zA-Z]+"))
                return true;
        }
        return false;
    }

    /**
     * get the name of stack or pop command
     * Stack var = ?x
     * Pop var = !x
     * 
     * @param s the string
     * @return substring of the name variable,
     * null otherwise
     */
    protected static String getStackOrPopVar(String s) {
        if (isPopVar(s) || isStackVar(s))
            return s.substring(1);
        else
            return null;
    }
}
