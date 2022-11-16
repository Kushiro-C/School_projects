/**
 * A simple class Ampoule, which represents a multicolor light bulb
 * The color is represented with RGB as 3 int values which are
 * between 0 and 255.
 * And also an intensity between 0 and 100.
 */
public class Ampoule {
    private int r; // Color Red
    private int g; // Color Green
    private int b; // Color Blue
    private double intensity; // Intensity of the light

    // Max RGB
    private final static int MAX_R = 255;
    private final static int MAX_G = 255;
    private final static int MAX_B = 255;
    private final static double MAX_INTENSITY = 100;

    /**
     * Constructor Ampoule
     * parameters RGB or intensity will be set to 0 by default
     * if the parameter is negative or bigger than MAX
     * @param r int Red >= 0
     * @param g int Green >= 0
     * @param b int Blue >= 0
     * @param intensity double Intensity of the light bulb >= 0
     */
    Ampoule(int r, int g, int b, double intensity) {
        this.r = (r < 0 || r > MAX_R) ? 0 : r;
        this.g = (g < 0 || g > MAX_G) ? 0 : g;
        this.b = (b < 0 || b > MAX_B) ? 0 : b;
        this.intensity =
            (intensity < 0 || intensity > MAX_INTENSITY) ?
            0 : intensity;
    }

    /**
     * Getter Red color
     * @return int r
     */
    public int getR() {
        return this.r;
    }

    /**
     * Getter Green color
     * @return int g
     */
    public int getG() {
        return this.g;
    }

    /**
     * Getter Blue color
     * @return int b
     */
    public int getB() {
        return this.b;
    }

    /**
     * Getter Intensity
     * @return double intensity
     */
    public double getIntensity() {
        return this.intensity;
    }

    /**
     * Setter Red color >= 0 and <= MAX_R
     * @param r int
     */
    public void setR(int r) {
        if (r >= 0 && r <= MAX_R)
            this.r = r;
    }

    /**
     * Setter Green color >= 0 and <= MAX_G
     * @param g int
     */
    public void setG(int g) {
        if (g >= 0 && g <= MAX_G)
            this.g = g;
    }

    /**
     * Setter Blue color >= 0 and <= MAX_B
     * @param b int
     */
    public void setB(int b) {
        if (b >= 0 && b <= MAX_B)
            this.b = b;
    }

    /**
     * Setter Intensity >= 0 and <= MAX_INTENSITY
     * @param intensity double
     */
    public void setIntensity(double intensity) {
        if (intensity >= 0 && intensity <= MAX_INTENSITY)
            this.intensity = intensity;
    }

    @Override
    public String toString() {
        String s = "Ampoule RGB(" + String.valueOf(r)
                    + ", " + String.valueOf(g)
                    + ", " + String.valueOf(b)
                    + "), Intensité : " + String.valueOf(intensity);

        return s;
    }
}
