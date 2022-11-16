package calc;

import org.junit.Test;
import static org.junit.Assert.*;

import java.math.BigDecimal;

public class CalculatorTest {
    @Test public void testAddition() {
        BigDecimal actual;
        BigDecimal expected;
        BigDecimal x, y;

        // Valid
        x = new BigDecimal("1.0");
        y = new BigDecimal("3.2");
        actual = Calculator.addition(x, y);
        expected = new BigDecimal("4.2");
        assertEquals(actual, expected);

        x = new BigDecimal("-1.0");
        y = new BigDecimal("2.5");
        actual = Calculator.addition(x, y);
        expected = new BigDecimal("1.5");
        assertEquals(actual, expected);

        x = new BigDecimal("1.0");
        y = new BigDecimal("-2.5");
        actual = Calculator.addition(x, y);
        expected = new BigDecimal("-1.5");
        assertEquals(actual, expected);

        x = new BigDecimal("0");
        y = new BigDecimal("0.0");
        actual = Calculator.addition(x, y);
        expected = new BigDecimal("0.0");
        assertEquals(actual, expected);

        // Not Valid
        expected = new BigDecimal("354.22"); // unexpected
        assertNotEquals(actual, expected);
    }

    @Test public void testSubstraction() {
        BigDecimal actual;
        BigDecimal expected;
        BigDecimal x, y;

        // Valid
        x = new BigDecimal("12.5");
        y = new BigDecimal("7.3");
        actual = Calculator.substraction(x, y);
        expected = new BigDecimal("5.2");
        assertEquals(actual, expected);

        x = new BigDecimal("2.2");
        y = new BigDecimal("3.3");
        actual = Calculator.substraction(x, y);
        expected = new BigDecimal("-1.1");
        assertEquals(actual, expected);

        x = new BigDecimal("-0.0");
        y = new BigDecimal("0.0");
        actual = Calculator.substraction(x, y);
        expected = new BigDecimal("0.0");
        assertEquals(actual, expected);

        // Not Valid
        x = new BigDecimal("7.8");
        y = new BigDecimal("-0.87");
        actual = Calculator.substraction(x, y);
        expected = new BigDecimal("25.2"); // unexpected
        assertNotEquals(actual, expected);
    }

    @Test public void testMultiplication() {
        BigDecimal actual;
        BigDecimal expected;
        BigDecimal x, y;

        // Valid
        x = new BigDecimal("0");
        y = new BigDecimal("58416.58");
        actual = Calculator.multiplication(x, y);
        expected = new BigDecimal("0.00");
        assertEquals(actual, expected);

        x = new BigDecimal("6584321.508");
        y = new BigDecimal("0");
        actual = Calculator.multiplication(x, y);
        expected = new BigDecimal("0.000");
        assertEquals(actual, expected);

        x = new BigDecimal("164.57");
        y = new BigDecimal("23.15");
        actual = Calculator.multiplication(x, y);
        expected = new BigDecimal("3809.7955");
        assertEquals(actual, expected);

        // Not Valid
        x = new BigDecimal("3.0");
        y = new BigDecimal("2.0");
        actual = Calculator.multiplication(x, y);
        expected = new BigDecimal("5.0"); // unexpected
        assertNotEquals(actual, expected);
    }

    @Test public void testDivision() {
        BigDecimal actual;
        BigDecimal expected;
        BigDecimal x, y;

        x = new BigDecimal("0.0");
        y = new BigDecimal("2384384.0546");
        actual = Calculator.division(x, y);
        expected = new BigDecimal("0E-12");
        assertEquals(actual, expected);

        x = new BigDecimal("1.0");
        y = new BigDecimal("3.0");
        actual = Calculator.division(x, y);
        expected = new BigDecimal("0.333333333333");
        assertEquals(actual, expected);

        x = new BigDecimal("5.2");
        y = new BigDecimal("-2.0");
        actual = Calculator.division(x, y);
        expected = new BigDecimal("-2.600000000000");
        assertEquals(actual, expected);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testDivisionByZero() {
        // This throws an IllegalArgumentException
        Calculator.division(new BigDecimal("42.42"), new BigDecimal("0.0"));
    }

    @Test public void testCalculate() {
        BigDecimal actual;
        BigDecimal expected;
        BigDecimal x, y;

        x = new BigDecimal("25.4");
        y = new BigDecimal("12.6");
        actual = Calculator.calculate(x, y, '+');
        expected = new BigDecimal("38");
        assertEquals(actual, expected);

        x = new BigDecimal("11.1");
        y = new BigDecimal("10.00");
        actual = Calculator.calculate(x, y, '-');
        expected = new BigDecimal("1.1");
        assertEquals(actual, expected);

        x = new BigDecimal("-3.0");
        y = new BigDecimal("5.1");
        actual = Calculator.calculate(x, y, '*');
        expected = new BigDecimal("-15.3");
        assertEquals(actual, expected);

        x = new BigDecimal("-8.4");
        y = new BigDecimal("-0.5");
        actual = Calculator.calculate(x, y, '/');
        expected = new BigDecimal("16.8");
        assertEquals(actual, expected);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testCalculateException () {
        BigDecimal a, zero;
        a = new BigDecimal("12.4");
        zero = new BigDecimal("0");

        Calculator.calculate(a, zero, 's'); // Operator not valid
        Calculator.calculate(a, zero, '/'); // Division by zero
    }


    //CaclulorInt test
    @Test public void testAdditionInt() {
        int actual;
        int expected;
        int x, y;

        // Valid
        x = 1;
        y = 3;
        actual = Calculator.addition(x, y);
        expected = 4;
        assertEquals(actual, expected);

        x = -1;
        y = 2;
        actual = Calculator.addition(x, y);
        expected = 1;
        assertEquals(actual, expected);

        x = 1;
        y = -2;
        actual = Calculator.addition(x, y);
        expected = -1;
        assertEquals(actual, expected);

        x = 0;
        y = 0;
        actual = Calculator.addition(x, y);
        expected = 0;
        assertEquals(actual, expected);

    }

    @Test public void testSubstractionInt() {
        int actual;
        int expected;
        int x, y;

        // Valid
        x = 12;
        y = 7;
        actual = Calculator.substraction(x, y);
        expected = 5;
        assertEquals(actual, expected);

        x = 2;
        y = 3;
        actual = Calculator.substraction(x, y);
        expected = -1;
        assertEquals(actual, expected);

        x = -0;
        y = 0;
        actual = Calculator.substraction(x, y);
        expected = 0;
        assertEquals(actual, expected);

    }

    @Test public void testMultiplicationInt() {
        int actual;
        int expected;
        int x, y;

        // Valid
        x = 0;
        y = 58416;
        actual = Calculator.multiplication(x, y);
        expected = 0;
        assertEquals(actual, expected);

        x = 6584321;
        y = 0;
        actual = Calculator.multiplication(x, y);
        expected = 0;
        assertEquals(actual, expected);

        x = 164;
        y = 23;
        actual = Calculator.multiplication(x, y);
        expected = 3772;
        assertEquals(actual, expected);

    }

    @Test public void testDivisionInt() {
        int actual;
        int expected;
        int x, y;

        x = 0;
        y = 2384384;
        actual = Calculator.division(x, y);
        expected = 0;
        assertEquals(actual, expected);

        x = 1;
        y = 3;
        actual = Calculator.division(x, y);
        expected = 0;
        assertEquals(actual, expected);

        x = 5;
        y = -2;
        actual = Calculator.division(x, y);
        expected = -2;
        assertEquals(actual, expected);

    }

    @Test(expected = IllegalArgumentException.class)
    public void testDivisionByZeroInt() {
        // This throws an IllegalArgumentException
        Calculator.division(42, 0);
    }

    @Test public void testCalculateInt() {
        int actual;
        int expected;
        int x, y;

        x = 25;
        y = 12;
        actual = Calculator.calculate(x, y, '+');
        expected = 37;
        assertEquals(actual, expected);

        x = 11;
        y = 10;
        actual = Calculator.calculate(x, y, '-');
        expected = 1;
        assertEquals(actual, expected);

        x = -3;
        y = 5;
        actual = Calculator.calculate(x, y, '*');
        expected = -15;
        assertEquals(actual, expected);

        x = -8;
        y = -2;
        actual = Calculator.calculate(x, y, '/');
        expected = 4;
        assertEquals(actual, expected); 
    }

    @Test(expected = IllegalArgumentException.class)
    public void testCalculateIntException () {
        int a, zero;
        a = 12;
        zero = 0;

        Calculator.calculate(a, zero, 's'); // Operator not valid
        Calculator.calculate(a, zero, '/'); // Division by zero
    }
}
