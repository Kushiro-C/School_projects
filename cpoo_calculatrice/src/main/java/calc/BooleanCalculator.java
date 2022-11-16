package calc;

/**
 * Handles the boolean operations
 * for our calculator.
 * @author Chang Patrick
 * @author Du Vincent
 */
public final class BooleanCalculator {

    private BooleanCalculator() {}

    /**
     * Boolean value of : x AND y
     * 
     * @param x boolean
     * @param y boolean
     * @return boolean
     */
    protected static boolean and(boolean x, boolean y) {
        return x && y;
    }

    /**
     * Boolean value of : x OR y
     * 
     * @param x boolean
     * @param y boolean
     * @return boolean
     */
    protected static boolean or(boolean x, boolean y) {
        return x || y;
    }

    /**
     * Boolean value of : NOT x
     * 
     * @param x boolean
     * @return boolean
     */
    protected static boolean not(boolean x) {
        return !x;
    }


    /**
     * Calculates the Boolean operation
     * between x and y, with the
     * Boolean operator op
     * 
     * @param x Boolean
     * @param y Boolean, can be null if op == "NON"
     * @param op operator ["ET", "OU", "NON"]
     * @return the result of the calculation
     * In case the op was not verified, then
     * this function will throw an exception
     */
    protected static boolean calculateBoolean(Boolean x, Boolean y, String op) {
        switch (op.toUpperCase()) {
            case "ET" :
                return and(x, y);
            case "OU" :
                return or(x, y);
            case "NON" :
                return not(x);
            default :
                break;
        }

        System.out.println("Operator not valid");
        throw new IllegalArgumentException("Operator not valid");
    }
}