package calc;

import org.junit.Test;
import static org.junit.Assert.*;


public class BooleanCalculatorTest {

    //BooleanCalculatorTest test
    @Test public void testAnd() {
        boolean actual;
        boolean expected;

        // Valid
        actual = BooleanCalculator.and(true, true);
        expected = true;
        assertEquals(actual, expected);

        actual = BooleanCalculator.and(false, true);
        expected = false;
        assertEquals(actual, expected);

        actual = BooleanCalculator.and(false, false);
        expected = false;
        assertEquals(actual, expected);

        actual = BooleanCalculator.and(true, false);
        expected = false;
        assertEquals(actual, expected);

        // Not Valid
        actual = BooleanCalculator.and(true, false);
        expected = true;
        assertNotEquals(actual, expected);
    }

    @Test public void testOr() {
        boolean actual;
        boolean expected;

        // Valid
        actual = BooleanCalculator.or(true, true);
        expected = true;
        assertEquals(actual, expected);

        actual = BooleanCalculator.or(false, true);
        expected = true;
        assertEquals(actual, expected);

        actual = BooleanCalculator.or(false, false);
        expected = false;
        assertEquals(actual, expected);

        actual = BooleanCalculator.or(true, false);
        expected = true;
        assertEquals(actual, expected);

        // Not Valid
        actual = BooleanCalculator.or(true, false);
        expected = false;
        assertNotEquals(actual, expected);
    }

    @Test public void testNot() {
        boolean actual;
        boolean expected;

        // Valid
        actual = BooleanCalculator.not(true);
        expected = false;
        assertEquals(actual, expected);

        actual = BooleanCalculator.not(false);
        expected = true;
        assertEquals(actual, expected);

        // Not Valid
        actual = BooleanCalculator.not(true);
        expected = true;
        assertNotEquals(actual, expected);
    }

    @Test public void testcalculateBoolean() {
        boolean actual;
        boolean expected;
        boolean x, y;

        x = true;
        y = false;
        actual = BooleanCalculator.calculateBoolean(x, y, "ET");
        expected = false;
        assertEquals(actual, expected);

        x = true;
        y = false;
        actual = BooleanCalculator.calculateBoolean(x, y, "OU");
        expected = true;
        assertEquals(actual, expected);

        x = true;
        actual = BooleanCalculator.calculateBoolean(x, x, "NON");
        expected = false;
        assertEquals(actual, expected);

    }

}
