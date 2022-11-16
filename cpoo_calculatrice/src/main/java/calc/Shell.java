package calc;

import java.util.*;
import java.math.BigDecimal;

/**
 * Class Shell
 * This class is the simple Shell
 * Waiting for the user to input data,
 * for our RPN Calculator and then handles
 * the data accordingly to the chosen type.
 * @author Chang Patrick
 * @author Du Vincent
 */
public class Shell {

    private final Type type;
    private final Stack<Object> stack;
    private final ArrayList<Object> history;
    private final HashMap<String, Object> variables;
    private static Scanner scan = new Scanner(System.in);

    private static enum Type {
        INTEGER, DOUBLE, BOOLEAN
    }

    // Constructors
    // For tests purpose, we leave them public,
    // But it should be private

    public Shell() {
        this.type = Type.DOUBLE;
        this.stack = new Stack<>();
        this.history = new ArrayList<>();
        this.variables = new HashMap<String, Object>();
    }

    public Shell(Type type) {
        this.type = type;
        this.stack = new Stack<>();
        this.history = new ArrayList<>();
        this.variables = new HashMap<String, Object>();
    }

    public Shell(Stack<BigDecimal> stack) {
        this.type = Type.DOUBLE;
        this.stack = new Stack<>();
        this.history = new ArrayList<>();
        this.variables = new HashMap<String, Object>();

        this.stack.addAll(stack);
    }

    public Shell(Stack<BigDecimal> stack, ArrayList<BigDecimal> history) {
        this.type = Type.DOUBLE;
        this.stack = new Stack<>();
        this.history = new ArrayList<>();
        this.variables = new HashMap<String, Object>();

        this.stack.addAll(stack);
        this.history.addAll(history);
    }

    public Shell(Stack<BigDecimal> stack, HashMap<String, BigDecimal> variables) {
        this.type = Type.DOUBLE;
        this.stack = new Stack<>();
        this.history = new ArrayList<>();
        this.variables = new HashMap<String, Object>();

        this.stack.addAll(stack);
        this.history.addAll(history);
        this.variables.putAll(variables);
    }

    /**
     * Prints an object,
     * The display is different according
     * to its type
     * 
     * @param o Object
     */
    private static void printValue(Object o) {
        if (o instanceof BigDecimal)
            System.out.println(((BigDecimal)o).toPlainString());
        else if (o instanceof Boolean) {
            String bool = (Boolean) o ? "VRAI" : "FAUX";
            System.out.println(bool);
        } else if (o instanceof Integer)
            System.out.println(o);
    }

    /**
     * Checks if the operation is
     * the boolean operator "NON"
     * 
     * @param op String
     * @return
     */
    private boolean isOpNon(String op) {
        return type.equals(Type.BOOLEAN) && op.toUpperCase().equals("NON");
    }

    /**
     * Checks if an operation is doable,
     * we look at the Stack, and if it has
     * enough elements, we can do an operation
     * 
     * @return true if we can do an operation
     */
    protected boolean canDoOperation() {
        return !(stack == null || stack.size() < 2);
    }

    /**
     * Checks if we can do the boolean operation
     * "NON", which requires the shell have a
     * type.BOOLEAN, and the at least one element
     * in the stack
     * 
     * @param op String
     * @return true if we can do "NON" operation
     */
    private boolean canDoNonOperation(String op) {
        return (type.equals(Type.BOOLEAN) && stack.size() > 0
            && op.toUpperCase().equals("NON"));
    }

    /**
     * This functions will do the operation
     * in parameter if possible, else it will
     * print a message explaining the error.
     * 
     * @param op String
     */
    protected void doOperation(String op) {
        if ((type.equals(Type.BOOLEAN) && !MyParser.validOperatorBoolean(op))
            || (!MyParser.isOperator(op) && !type.equals(Type.BOOLEAN)))
            System.out.println("Input not valid");
        else if (!canDoOperation() && !canDoNonOperation(op))
            System.out.println("Stack does not contain enough elements");
        else {
            Object x = stack.pop();
            Object y = isOpNon(op) ? null : stack.pop();

            // Doing operation according to the type
            try {
                Object result = null;
                switch (type) {
                    case INTEGER :
                        result = Calculator.calculate(
                                (Integer)y, (Integer)x, op.charAt(0));
                        break;

                    case DOUBLE :
                        result = Calculator.calculate(
                                (BigDecimal)y, (BigDecimal)x, op.charAt(0));
                        break;

                    case BOOLEAN :
                        result = BooleanCalculator.calculateBoolean(
                                (Boolean)x, (Boolean)y, op);
                        break;

                }

                if (result != null) {
                    stack.push(result);
                    history.add(result);
                    printValue(result);
                }

            } catch (IllegalArgumentException e) {
                if (y != null)
                    stack.push(y);
                stack.push(x);
            }
        }
    }

    /**
     * This functions will find the variable at index
     * in history if it is possible, else it will
     * print a message explaining the error.
     * @param index String
     */
    protected void findHist(String index) {
        int i = Integer.parseInt(MyParser.getHistOrPile(index));
        int size = history.size();

        if (size == 0 || i >= size || (i < 0 && i < -(size)))
                System.out.println("History does not contain that many elements");
        else {
            if (i >= 0) {
                Object hist = history.get(i);
                printValue(hist);
                stack.push(hist);
                history.add(hist);
            } else {
                Object hist = history.get(history.size() + i);
                printValue(hist);
                stack.push(hist);
                history.add(hist);
            }
        }
    }

    /**
     * This functions will find the variable at index
     * in the stack if it is possible, else it will
     * print a message explaining the error.
     * @param index String
     */
    protected void findPile(String index) {
        int i = Integer.parseInt(MyParser.getHistOrPile(index));
        int size = stack.size();

        if (size == 0 || i >= size || (i < 0 && i < -(size)))
                System.out.println("Stack does not contain that many elements");
        else {
            if (i >= 0) {
                Object stac = stack.get(i);
                System.out.println(stac);
                stack.push(stac);
                history.add(stac);
            } else {
                Object stac = stack.get(stack.size() + i);
                System.out.println(stac);
                stack.push(stac);
                history.add(stac);
            }
        }
    }

    /**
     * This functions will find the variable
     * in varaibles if it is possible, else it will
     * print a message explaining the error.
     * @param var String
     */
    protected void findVariables(String var) {
        String name = MyParser.getStackOrPopVar(var);

        if (!variables.containsKey(name))
                System.out.println("Variable not found");
        else {
            printValue(variables.get(name));
            stack.push(variables.get(name));
            history.add(variables.get(name));
        }
    }

    /**
     * This functions will pop the numeric
     * in stack if it is possible, else it will
     * print a message explaining the error.
     * @param line String
     */
    protected void popVariables(String line) {
        if (stack.size() >= 1) {
            if (variables.containsKey(MyParser.getStackOrPopVar(line)))
                variables.remove(MyParser.getStackOrPopVar(line));
            variables.put(MyParser.getStackOrPopVar(line), stack.pop());
            if (stack.empty())
                System.out.println("Empty stack");
            else
                printValue(stack.peek());
        } else
            System.out.println("Stack does not contain enough elements");
    }

    /**
     * Launches the mini Shell :
     * It waits for the user to input data,
     * then calculates evaluates the data
     * as a simple RPN calculator
     * Handles the types [decimals, integers, booleans]
     */
    private static void operate(Type type) {
        String line;
        Shell mySh = new Shell(type);

        try {
            while(true) {
                System.out.print("> ");
                line = scan.nextLine(); // User input

                if (type.equals(Type.DOUBLE) && MyParser.isDouble(line)) {
                    mySh.stack.push(new BigDecimal(line).stripTrailingZeros());
                    mySh.history.add(new BigDecimal(line).stripTrailingZeros());
                    printValue(mySh.stack.peek());
                }
                else if (type.equals(Type.INTEGER) && MyParser.isInt(line)) {
                    mySh.stack.push(Integer.valueOf(line));
                    mySh.history.add(Integer.valueOf(line));
                    printValue(mySh.stack.peek());
                }
                else if (type.equals(Type.BOOLEAN) && MyParser.isBoolean(line)) {
                    mySh.stack.push(MyParser.getBoolean(line));
                    mySh.history.add(MyParser.getBoolean(line));
                    printValue(mySh.stack.peek());
                }
                else if (MyParser.isHist(line)) {
                    mySh.findHist(line);
                }
                else if (MyParser.isPile(line)) {
                    mySh.findPile(line);
                }
                else if (MyParser.isPopVar(line)) {
                    mySh.popVariables(line);
                }
                else if (MyParser.isStackVar(line)) {
                    mySh.findVariables(line);
                }
                else if (line.equals("q"))
                    break;
                else
                    mySh.doOperation(line);
            }

        } catch (IllegalStateException | NoSuchElementException e) {
            System.out.println("Program Terminated");
            scan.close();
        }
    }

    /**
     * This function asks the user to select the type of
     * the values he wants to use in this RPN Calculator.
     * Available types are :
     * [decimals (default), integers, booleans]
     * It will then launch the actual Shell and Calculator.
     */
    public static void selectTypeAndLaunch() {
        Type selectedType = null;
        String userInput;

        System.out.println("Hello and welcome to our RPN Calculator !\n"
            + "Please select the type of value you want to use :\n"
            + "Press [ENTER] for default (decimals)\n"
            + "-> Enter [1] for decimals\n"
            + "-> Enter [2] for integers\n"
            + "-> Enter [3] for booleans");

        try {
            // Get user selected type
            userInput = scan.nextLine();
            while (selectedType == null) {
                switch (userInput) {
                    case "" :
                    case "1" :
                        selectedType = Type.DOUBLE;
                        System.out.println("You chose decimals");
                        break;
                    case "2" :
                        selectedType = Type.INTEGER;
                        System.out.println("You chose integers");
                        break;
                    case "3" :
                        System.out.println("You chose booleans");
                        selectedType = Type.BOOLEAN;
                        break;
                    default :
                        userInput = scan.nextLine();

                }
            }
        } catch (IllegalStateException | NoSuchElementException e) {
            scan.close();
        }


        operate(selectedType);
        scan.close();
    }
}
