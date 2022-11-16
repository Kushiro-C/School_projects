package calc;

import org.junit.Test;
import static org.junit.Assert.*;

public class MyParserTest {
    @Test public void testValidOperator() {
        // Valid operators
        assertTrue(MyParser.validOperator('+'));
        assertTrue(MyParser.validOperator('-'));
        assertTrue(MyParser.validOperator('*'));
        assertTrue(MyParser.validOperator('/'));

        // Not Valid operators
        assertFalse(MyParser.validOperator('a'));
        assertFalse(MyParser.validOperator(' '));
        assertFalse(MyParser.validOperator('1'));
    }

    @Test public void testIsOperator() {
        // Valid operators
        assertTrue(MyParser.isOperator("+"));
        assertTrue(MyParser.isOperator("-"));
        assertTrue(MyParser.isOperator("*"));
        assertTrue(MyParser.isOperator("/"));

        // Not operators
        assertFalse(MyParser.isOperator("2"));
        assertFalse(MyParser.isOperator(""));
        assertFalse(MyParser.isOperator(" "));
        assertFalse(MyParser.isOperator("azekazjn jk"));
    }

    @Test public void testValidOperatorBoolean() {
        // Valid operators
        assertTrue(MyParser.validOperatorBoolean("ET"));
        assertTrue(MyParser.validOperatorBoolean("OU"));
        assertTrue(MyParser.validOperatorBoolean("NON"));
        assertTrue(MyParser.validOperatorBoolean("non"));

        // Not Valid operators
        assertFalse(MyParser.validOperatorBoolean("a"));
        assertFalse(MyParser.validOperatorBoolean(" "));
    }

    @Test public void testIsBoolean() {
        // Convertable as boolean
        assertTrue(MyParser.isBoolean("VRAI"));
        assertTrue(MyParser.isBoolean("FAUX"));
        assertTrue(MyParser.isBoolean("Faux"));

        // Not convertable as boolean
        assertFalse(MyParser.isBoolean("VRAIFAUX"));
        assertFalse(MyParser.isBoolean("fz"));
    }

    @Test public void testGetBoolean() {
        // Supposed convertable as boolean
        assertTrue(MyParser.getBoolean("VRAI"));
        assertFalse(MyParser.getBoolean("FAUX"));
    }

    @Test public void testIsDouble() {
        // Convertable as numbers
        assertTrue(MyParser.isDouble("36"));
        assertTrue(MyParser.isDouble("0"));
        assertTrue(MyParser.isDouble("-12"));
        assertTrue(MyParser.isDouble("0.00"));
        assertTrue(MyParser.isDouble("1351.654"));
        assertTrue(MyParser.isDouble("-1351.654"));

        // Not convertable as numbers
        assertFalse(MyParser.isDouble("-"));
        assertFalse(MyParser.isDouble("12..25"));
        assertFalse(MyParser.isDouble(" 32.5  "));
        assertFalse(MyParser.isDouble("1 12.5 "));
        assertFalse(MyParser.isDouble("12.s5"));
        assertFalse(MyParser.isDouble("!12.5"));
    }

    @Test public void testIsInt() {
        // Convertable as numbers
        assertTrue(MyParser.isInt("36"));
        assertTrue(MyParser.isInt("-36"));
        assertTrue(MyParser.isInt("0"));
        assertTrue(MyParser.isInt("-000"));
        assertTrue(MyParser.isInt("00135154"));

        // Not convertable as numbers
        assertFalse(MyParser.isInt("-"));
        assertFalse(MyParser.isInt("--36"));
        assertFalse(MyParser.isInt(" 35  "));
        assertFalse(MyParser.isInt("-1351.654"));
        assertFalse(MyParser.isInt(""));
        assertFalse(MyParser.isInt(null));
        assertFalse(MyParser.isInt("12s5"));
        assertFalse(MyParser.isInt("!15"));
    }

    @Test public void testIsHist() {
        // Is hist command
        assertTrue(MyParser.isHist("hist(0)"));
        assertTrue(MyParser.isHist("hist(4766)"));
        assertTrue(MyParser.isHist("hist(-4766)"));

        // Not pile command
        assertFalse(MyParser.isHist("hist()"));
        assertFalse(MyParser.isHist("hist(1234.11)"));
        assertFalse(MyParser.isHist("hist( 32.5  )"));
        assertFalse(MyParser.isHist("hist(--10)"));
        assertFalse(MyParser.isHist("hist(zzza)"));
        assertFalse(MyParser.isHist("hist("));
        assertFalse(MyParser.isHist("hist"));
        assertFalse(MyParser.isHist("hist)"));
        assertFalse(MyParser.isHist("hist(1 12.5 )"));
        assertFalse(MyParser.isHist("hist(12.s5)"));
        assertFalse(MyParser.isHist("hist(!12.5)"));
    }

    @Test public void testgetHistOrPile() {
        // get hist numeric substring
        assertEquals("0" , MyParser.getHistOrPile("hist(0)"));
        assertEquals("4766", MyParser.getHistOrPile("hist(4766)"));
        assertEquals("-25", MyParser.getHistOrPile("pile(-25)"));
        assertEquals("000" , MyParser.getHistOrPile("pile(000)"));

        // null because not hist or pile
        assertEquals(null, MyParser.getHistOrPile("hist(0.11)"));
        assertEquals(null, MyParser.getHistOrPile("hist(1234.11)"));
        assertEquals(null, MyParser.getHistOrPile("hist( 32.5  )"));
        assertEquals(null, MyParser.getHistOrPile("pile(0.11)"));
        assertEquals(null, MyParser.getHistOrPile("pile(1234.11)"));
        assertEquals(null, MyParser.getHistOrPile("pile( 32.5  )"));
        assertEquals(null, MyParser.getHistOrPile("hist()"));
        assertEquals(null, MyParser.getHistOrPile("hist(zzza)"));
        assertEquals(null, MyParser.getHistOrPile("hist("));
        assertEquals(null, MyParser.getHistOrPile("hist"));
        assertEquals(null, MyParser.getHistOrPile("hist)"));
        assertEquals(null, MyParser.getHistOrPile("hist(!12.5)"));
        assertEquals(null, MyParser.getHistOrPile("pile(!12.5)"));
        assertEquals(null, MyParser.getHistOrPile("pile("));
        assertEquals(null, MyParser.getHistOrPile("pile"));
        assertEquals(null, MyParser.getHistOrPile("pile)"));
        assertEquals(null, MyParser.getHistOrPile("pilehist(4)"));
    }

    @Test public void testIsPile() {
        // Is pile command
        assertTrue(MyParser.isPile("pile(0)"));
        assertTrue(MyParser.isPile("pile(4766)"));
        assertTrue(MyParser.isPile("pile(-33)"));

        // Not pile command
        assertFalse(MyParser.isPile("pile(0.11)"));
        assertFalse(MyParser.isPile("pile(--12341)"));
        assertFalse(MyParser.isPile("pile( 32.5  )"));
        assertFalse(MyParser.isPile("pile()"));
        assertFalse(MyParser.isPile("pile(zzza)"));
        assertFalse(MyParser.isPile("pile("));
        assertFalse(MyParser.isPile("pile"));
        assertFalse(MyParser.isPile("pile)"));
        assertFalse(MyParser.isPile("pile(1 12.5 )"));
        assertFalse(MyParser.isPile("pile(12.s5)"));
        assertFalse(MyParser.isPile("pile(!12.5)"));
    }

    @Test public void testIsPopVar() {
        // Pop a variable
        assertTrue(MyParser.isPopVar("!az"));
        assertTrue(MyParser.isPopVar("!x"));

        // Not Pop a variable
        assertFalse(MyParser.isPopVar("!11x"));
        assertFalse(MyParser.isPopVar("!x1"));
        assertFalse(MyParser.isPopVar("!x "));
        assertFalse(MyParser.isPopVar("!"));
        assertFalse(MyParser.isPopVar("!a2&"));
        assertFalse(MyParser.isPopVar("!&"));
        assertFalse(MyParser.isPopVar("aaa"));
    }

    @Test public void testIsStackVar() {
        // Pop a variable
        assertTrue(MyParser.isStackVar("?az"));
        assertTrue(MyParser.isStackVar("?x"));

        // Not Pop a variable
        assertFalse(MyParser.isStackVar("?11x"));
        assertFalse(MyParser.isStackVar("?x1"));
        assertFalse(MyParser.isStackVar("?x "));
        assertFalse(MyParser.isStackVar("?"));
        assertFalse(MyParser.isStackVar("?a2&"));
        assertFalse(MyParser.isStackVar("?&"));
        assertFalse(MyParser.isStackVar("aaa"));
    }

    @Test public void testGetStackOrPopVar() {
        //get names
        assertEquals("az", MyParser.getStackOrPopVar("?az"));
        assertEquals("x", MyParser.getStackOrPopVar("?x"));
        assertEquals("az", MyParser.getStackOrPopVar("!az"));
        assertEquals("x", MyParser.getStackOrPopVar("!x"));


        // Not Pop a variable
        assertEquals(null, MyParser.getStackOrPopVar("?11x"));
        assertEquals(null, MyParser.getStackOrPopVar("?"));
        assertEquals(null, MyParser.getStackOrPopVar("?x1"));
        assertEquals(null, MyParser.getStackOrPopVar("! x"));
        assertEquals(null, MyParser.getStackOrPopVar("?a2&"));
        assertEquals(null, MyParser.getStackOrPopVar("?&"));
        assertEquals(null, MyParser.getStackOrPopVar("aaa"));
        assertEquals(null, MyParser.getStackOrPopVar("!x "));
        assertEquals(null, MyParser.getStackOrPopVar("!"));
        assertEquals(null, MyParser.getStackOrPopVar("!a2&"));
        assertEquals(null, MyParser.getStackOrPopVar("!&"));
    }
}
