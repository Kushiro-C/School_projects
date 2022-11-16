package calc;

import org.junit.Before;
import org.junit.After;
import org.junit.Test;
import static org.junit.Assert.*;

import java.util.*;
import java.math.BigDecimal;
import java.io.ByteArrayOutputStream;
import java.io.PrintStream;

public class ShellTest {
    private final ByteArrayOutputStream outContent = new ByteArrayOutputStream();
    private final PrintStream oldOut = System.out;

    @Before
    public void setUpStream() {
        System.setOut(new PrintStream(outContent));
    }

    @After
    public void restoreStream() {
        System.setOut(oldOut);
    }

    @Test public void testCanDoOperation() {
        // Testing with empty Stack
        Shell testShellEmpty = new Shell();
        assertFalse(testShellEmpty.canDoOperation());

        // One Element in the Stack
        Stack<BigDecimal> testStack = new Stack<BigDecimal>();
        testStack.push(new BigDecimal("1.1"));
        Shell testShell = new Shell(testStack);

        assertFalse(testShell.canDoOperation());

        // Two Elements in the Stack
        testStack.push(new BigDecimal("2.2"));
        assertFalse(testShell.canDoOperation()); // avoid aliasing

        Shell testShell2 = new Shell(testStack);
        assertTrue(testShell2.canDoOperation());
    }

    @Test public void testDoOperation() {
        Stack<BigDecimal> testStack = new Stack<>();

        // Adding elements to the Stack
        testStack.push(new BigDecimal("-3.3"));
        testStack.push(new BigDecimal("-1.0"));
        testStack.push(new BigDecimal("2.2"));
        testStack.push(new BigDecimal("3.3"));
        testStack.push(new BigDecimal("0.0"));

        Shell testShell = new Shell(testStack);


        // All different cases of output
        testShell.doOperation("/"); // Division by zero
        testShell.doOperation("+"); // 3.3
        testShell.doOperation("-"); // -1.1
        testShell.doOperation("*"); // 1.1
        testShell.doOperation("/"); // -3
        testShell.doOperation("aze"); // Input not valid
        testShell.doOperation("+"); // Stack does not contain enough elements

        String expected =
            "Division by zero\n" +
            "3.3\n-1.1\n1.1\n-3\n" +
            "Input not valid\n" +
            "Stack does not contain enough elements\n";

        assertEquals(expected, outContent.toString());
    }

    @Test public void testFindHist() {

        Stack<BigDecimal> testStack = new Stack<>();
        ArrayList<BigDecimal> testHistoric = new ArrayList<>();

        // Adding elements to the Stack and ArrayList
        testStack.push(new BigDecimal("4"));
        testStack.push(new BigDecimal("15"));
        testHistoric.add(new BigDecimal("4"));
        testHistoric.add(new BigDecimal("15"));

        Shell testShell = new Shell(testStack, testHistoric);

        // and All different cases of output
        testShell.findHist("hist(-3)"); // find out of bound
        testShell.findHist("hist(2)"); // re-find out of bound
        testShell.findHist("hist(-1)"); // find the last element that we pushed
        testShell.findHist("hist(-2)"); // find 2 elements before in history
        testShell.findHist("hist(-4)"); // find 4 elements before in history
        testShell.doOperation("+"); // operation : 15 + 4 = 19

        String expected =
            "History does not contain that many elements\n" +
            "History does not contain that many elements\n" +
            "15\n" + "15\n" + "4\n" +
            "19\n";

        assertEquals(expected, outContent.toString());
    }

    @Test public void testFindPile() {

        Stack<BigDecimal> testStack = new Stack<>();
        ArrayList<BigDecimal> testPile = new ArrayList<>();

        // Adding elements to the Stack and ArrayList
        testStack.push(new BigDecimal("4"));
        testStack.push(new BigDecimal("15"));
        testPile.add(new BigDecimal("4"));
        testPile.add(new BigDecimal("15"));

        Shell testShell = new Shell(testStack, testPile);

        //and All different cases of output
        testShell.findPile("pile(-3)"); // find out of bound
        testShell.findPile("pile(-3)"); // refind out of bound
        testShell.findPile("pile(-1)"); // find the last element that we pushed
        testShell.findPile("pile(-2)"); // find 2 element before
        testShell.findPile("pile(-4)"); // find 4 element before
        testShell.doOperation("+"); // ° operation

        String expected =
            "Stack does not contain that many elements\n" +
            "Stack does not contain that many elements\n" +
            "15\n" + "15\n" + "4\n" +
            "19\n";

        assertEquals(expected, outContent.toString());
    }

    @Test public void testFindAndPopVariables() {

        Stack<BigDecimal> testStack = new Stack<>();

        // Adding elements to the Stack and ArrayList
        testStack.push(new BigDecimal("4"));
        testStack.push(new BigDecimal("1"));
        testStack.push(new BigDecimal("3"));

        Shell testShell = new Shell(testStack);

        // and All different cases of output
        testShell.findVariables("?y"); // find y variable
        testShell.popVariables("!x"); // store 3, print new top of stack 1
        testShell.popVariables("!x"); // store 1, print new top of stack 4
        testShell.findVariables("?x"); // find x variable, new top of stack 1
        testShell.doOperation("+"); // do + opration : 5
        testShell.findVariables("?x"); // find x variable, new top of stack 1
        testShell.doOperation("-"); // do + opration : 4
        testShell.popVariables("!x"); // store 4, print Empty stack

        String expected =
            "Variable not found\n" +
            "1\n" + "4\n" + // pop var, top stack new value
            "1\n" + "5\n" + // find var then do operation
            "1\n" + "4\n" + // find var then do operation
            "Empty stack\n";

        assertEquals(expected, outContent.toString());
    }
}
