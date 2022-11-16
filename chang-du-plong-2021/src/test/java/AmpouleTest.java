import org.junit.Test;
import static org.junit.Assert.*;

public class AmpouleTest {

    // Test Constructor Normal
    @Test public void testConstructor() {
        Ampoule testAmpoule = new Ampoule(42, 0, 255, 100);
        assertEquals(42, testAmpoule.getR());
        assertEquals(0, testAmpoule.getG());
        assertEquals(255, testAmpoule.getB());
        assertEquals(100, testAmpoule.getIntensity(), 0.01);
    }

    // Test Constructor with wrong parameters (auto set to 0 as default)
    @Test public void testConstructorAutoDefault() {
        Ampoule testAmpouleOverMax = new Ampoule(256, 257, 30551, 100.1);
        Ampoule testAmpouleNegative = new Ampoule(-1, -35120, -10, -0.1);
        assertEquals(0, testAmpouleOverMax.getR());
        assertEquals(0, testAmpouleOverMax.getG());
        assertEquals(0, testAmpouleOverMax.getB());
        assertEquals(0, testAmpouleOverMax.getIntensity(), 0.01);

        assertEquals(0, testAmpouleNegative.getR());
        assertEquals(0, testAmpouleNegative.getG());
        assertEquals(0, testAmpouleNegative.getB());
        assertEquals(0, testAmpouleNegative.getIntensity(), 0.01);
    }

    // Test Setters with acceptable values

    @Test public void testSetRedColor() {
        Ampoule testAmpoule = new Ampoule(42, 42, 42, 25);
        testAmpoule.setR(50);
        assertEquals(50, testAmpoule.getR());
    }

    @Test public void testSetGreenColor() {
        Ampoule testAmpoule = new Ampoule(42, 42, 42, 25);
        testAmpoule.setG(60);
        assertEquals(60, testAmpoule.getG());
    }

    @Test public void testSetBlueColor() {
        Ampoule testAmpoule = new Ampoule(42, 42, 42, 25);
        testAmpoule.setB(75);
        assertEquals(75, testAmpoule.getB());
    }

    @Test public void testSetIntensity() {
        Ampoule testAmpoule = new Ampoule(42, 42, 42, 25);
        testAmpoule.setIntensity(80.33);
        assertEquals(80.33, testAmpoule.getIntensity(), 0.01);
    }

    // Test Setters with Out of range values

    @Test public void testSetRedColorFail() {
        Ampoule testAmpoule = new Ampoule(42, 42, 42, 25);
        testAmpoule.setR(-1);
        assertNotEquals(-1, testAmpoule.getR());
        testAmpoule.setR(256);
        assertEquals(42, testAmpoule.getR());
    }

    @Test public void testSetGreenColorFail() {
        Ampoule testAmpoule = new Ampoule(42, 42, 42, 25);
        testAmpoule.setG(-1);
        assertNotEquals(-1, testAmpoule.getG());
        testAmpoule.setG(256);
        assertEquals(42, testAmpoule.getG());
    }

    @Test public void testSetBlueColorFail() {
        Ampoule testAmpoule = new Ampoule(42, 42, 42, 25);
        testAmpoule.setB(-1);
        assertNotEquals(-1, testAmpoule.getB());
        testAmpoule.setB(256);
        assertEquals(42, testAmpoule.getB());
    }

    @Test public void testSetIntensityFail() {
        Ampoule testAmpoule = new Ampoule(42, 42, 42, 25);
        testAmpoule.setIntensity(-1);
        assertNotEquals(-1, testAmpoule.getIntensity(), 0.01);
        testAmpoule.setIntensity(101);
        assertEquals(25, testAmpoule.getIntensity(), 0.01);
    }
}
