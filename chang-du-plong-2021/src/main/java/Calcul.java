import java.util.HashMap;

import javax.sound.sampled.UnsupportedAudioFileException;

import java.awt.Color;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.io.*;
import java.math.BigDecimal;
import java.math.RoundingMode;
import java.sql.*;
import org.jtransforms.fft.DoubleFFT_1D;

public class Calcul {

    private final static int subSize = 4096; // need a value that never be odd during fft
    private static int[] range = new int[] { 40, 80, 120, 180, 300 }; // most common frequency
    private static Color[] color = initColor(); // new Color[]{Color.BLUE, Color.CYAN, Color.YELLOW, Color.ORANGE, Color.RED};
    private static int rangeMin = range[0];
    private static int rangeMax = range[range.length - 1];
    private final static int sampleRate = 44100; // Hz
    private final static Arduino arduino = new Arduino(); // if use arduino
    private final static boolean isArduinoHere = arduino.initialize();
    //private final static boolean isArduinoHere = false;
    //private final static Arduino arduino = null; //if not use arduino, please uncomment

    // fast fourier transform, FFT of x[]
    // transform time domain to frequency
    // (check file "annexes")
    public static Complex[] fft(Complex[] x) {
        int n = x.length;

        if (n == 1)
            return new Complex[] { x[0] };

        if (n % 2 != 0) {
            throw new IllegalArgumentException("n is not a power of 2");
            // return null;
        }

        // FFT of even
        Complex[] even = new Complex[n / 2];
        for (int i = 0; i < n / 2; i++) {
            even[i] = x[2 * i];
        }
        Complex[] evenRec = fft(even);

        // FFT of odd
        Complex[] odd = even;
        for (int i = 0; i < n / 2; i++) {
            odd[i] = x[2 * i + 1];
        }
        Complex[] oddRec = fft(odd);

        // fusion
        Complex[] y = new Complex[n];
        for (int i = 0; i < n / 2; i++) {
            double kth = -2 * i * Math.PI / n;
            Complex wk = new Complex(Math.cos(kth), Math.sin(kth));
            y[i] = evenRec[i].plus(wk.times(oddRec[i]));
            y[i + n / 2] = evenRec[i].minus(wk.times(oddRec[i]));
        }
        return y;
    }

    /**
     * return in which range the frequency is in { 40, 80, 120, 180, 300 };
     * @param freq
     * @return
     */
    public static int getIndexRange(int freq) {
        int i = 0;
        while (range[i] < freq) {
            i++;
            if (i >= range.length)
                return i - 1;
        }
        return i;
    }

    /**
     * take byte[] of audio and divide it by subSize
     * then apply fft on it
     *
     * @param audio
     * @return double[audio.length / subSize][subSize]
     */
    public static Complex[][] applyFFTOnByteArray(byte audio[]) {
        int sampleSize = audio.length / subSize; // size array of sub audio
        Complex[][] resultFFT = new Complex[sampleSize][subSize];

        // applying fast fourier transform on sub array
        for (int i = 0; i < sampleSize; i++) {
            Complex[] complex = new Complex[subSize];

            for (int j = 0; j < subSize; j++) {
                complex[j] = new Complex(audio[j + (i * subSize)], 0.0); // imaginary = 0
            }

            resultFFT[i] = Calcul.fft(complex);
        }
        return resultFFT;
    }

    /**
     * take byte[] of audio and aply fft
     *
     * @param audio
     * @return double[audio.length / subSize][subSize]
     */
    public static Complex[] applyFFTOnByteArrayWithoutDivide(byte audio[]) throws UnsupportedAudioFileException {
        double[] samples = Recorder.byteArrayToDoubleArrayAudio(audio);
        Complex[] resultFFT = new Complex[samples.length];
        double maxPeak = 0;

        // Get max peak of samples
        for (int i = 0; i < subSize; i++)
            maxPeak = samples[i] > maxPeak ? samples[i] : maxPeak;

        // applying fast fourier transform
        Complex[] complex = new Complex[subSize];
        for (int j = 0; j < subSize; j++) {
            complex[j] = new Complex(normalize(samples[j], maxPeak), 0.0); // imaginary = 0
        }
        resultFFT = Calcul.fft(complex);
        return resultFFT;
    }


    /**
     * Calculates the rate of number of time the audio sample crosses 0
     * @param data audio in double (without applying FFT)
     * @return (double) zero-crossing rate
     */
    public static double zeroCrossingRate(double[] data) {
        if (data.length < 1)
            return 0;

        double cpt = 0;
        boolean neg = data[0] < 0 ? true : false; // Last is negative

        // Check only real part of signal
        for (int i = 1; i < data.length; i++) {
            // Increment ZC counter
            if (!neg && data[i] < 0) {
                neg = true;
                cpt++;
            } else if (neg && data[i] >= 0) {
                neg = false;
                cpt++;
            }
        }

        // System.out.println("Zero Crossing Rate = " + cpt);

        return cpt;
    }

    /**
     * Normalizes an audio value
     * @param value Samples[i] value
     * @param rawMax peak sample
     * @return Normalized value
     */
    public static double normalize(double value, double rawMax) {
        double targetMax = 700;

        // This is the maximum volume reduction
        double maxReduce = 1 - targetMax/(double)rawMax;

        int abs = Math.abs((short) value);
        double factor = (maxReduce * abs/rawMax);

        return (double) Math.round((1 - factor) * value);
    }

    /**
     * Get all magnitudes from a FFT'ed array
     * It uses wendykierp's JTransform library for FFT
     * @param fft_out transformed samples
     * @return all magnitudes from the samples
     */
    public static double[] getAllMagnitudes(double[] fft_out) {
        double magnitudes[] = new double[fft_out.length / 2];
        // double maxMagnitude = 0;

        for (int i = 0; i < magnitudes.length; i++) {
            magnitudes[i] = Math.sqrt(fft_out[2*i] * fft_out[2*i]
                                + fft_out[2*i+1] * fft_out[2*i+1]);

            // maxMagnitude = magnitudes[i] > maxMagnitude ? magnitudes[i] : maxMagnitude;
        }

        return magnitudes;
    }

    /**
     * Count the number of peaks above the threshold
     * It uses wendykierp's JTransform library for FFT
     * @param magnitudes from samples
     * @param threshold only peaks above this will matter
     * @return the number of magnitudes above the given threshold
     */
    public static int getNbAboveThreshold(double[] magnitudes, double threshold) {
        int cpt = 0;

        // Nyquist–Shannon sampling theorem :
        // Only first half frequencies of samplingRate are important
        for (int i = 0; i < magnitudes.length / 2; i++) {
            if (magnitudes[i] >= threshold)
                cpt++;
        }

        return cpt;
    }

    /**
     * Tests if a voice is playing in the samples
     * @param samples[]
     * @param magnitudes[]
     * @return boolean
     */
    public static boolean isVoicePlaying(double samples[], double magnitudes[]) {
        // Test Zero Crossing Rate
        double zcr = Calcul.zeroCrossingRate(samples);
        boolean voiceZCR = zcr < 170 && zcr > 15;

        // Get all magnitudes over the threshold
        double threshold = 70000;
        int counter = getNbAboveThreshold(magnitudes, threshold);
        boolean voiceMag = counter > 50;

        return voiceZCR && voiceMag;
    }

    /**
     * Generate color and set voice vue
     * It uses wendykierp's JTransform library for FFT
     * @param audio
     * @param vue
     * @return Color
     */
    public static Color voiceColorGenerator(double samples[], Vue vue) {
        double peak = 0;
        int len = samples.length;
        final double maxIntensity = 40000;
        double intensity = 0;
        double sumSamples = 0;

        // Get peak sample
        for (int i = 0; i < len; i++) {
            if (Math.abs(samples[i]) > peak)
                peak = Math.abs(samples[i]);
        }

        // Get intensity
        for (int i = 0; i < len; i++)
            sumSamples += (samples[i] * samples[i]) / 32767;

        intensity = (sumSamples / len) * 100 / maxIntensity;
        intensity = roundDouble(intensity);

        // Normalize samples audio
        for (int i = 0; i < len; i++) {
            // Filters most BG buzzing noise
            samples[i] = normalize(samples[i], peak);
        }

        // Apply fft to samples
        DoubleFFT_1D fft_1d = new DoubleFFT_1D(len);
        double[] fft_out = new double[len * 2];
        System.arraycopy(samples, 0, fft_out, 0, len);
        fft_1d.realForwardFull(fft_out);

        // Get all magnitudes
        double magnitudes[] = getAllMagnitudes(fft_out);
        boolean isVoice = isVoicePlaying(samples, magnitudes);

        // Set voice color to default
        if (!isVoice) {
            vue.setVoiceRGB(42, 42, 42);
            vue.setVoiceIntensity(0);
            return new Color(42, 42, 42);
        }

        // Voice detected -> Change voice color

        double maxMagnitude = -1;
        double maxFreq = 0;
        double freq = 0;

        for (int i = 0; i < magnitudes.length; i++) {
            // ignore low sound
            if (magnitudes[i] < Vue.getSliderValue())
                continue;

            // keep highest magnitude
            // and its corresponding frequency if between 40 Hz and 500 Hz
            if (magnitudes[i] > maxMagnitude) {
                freq = (i * sampleRate) / magnitudes.length;
                if (freq > 500 || freq < 40)
                    continue;

                maxMagnitude = magnitudes[i];
                maxFreq = freq;
            }
        }

        Color result;

        // index of color inside array colors[1020]
        int i = (int) ((maxFreq - 40) * 2.3);

        // put in result appropriate color
        if (i >= 0 && i < color.length - 1)
            result = color[i];
        else if (maxFreq <= 0) // default color low frequency
            result = color[0];
        else // default color high frequency
            result = color[color.length - 1];

        // Set voice panel
        vue.setVoiceRGB(result.getRed(), result.getGreen(), result.getBlue());
        vue.setVoiceIntensity(intensity);

        return result;

    }

    /**
     * generate color and set music vue
     *
     * @param audio
     * @param vue
     * @return Color
     */
    public static Color colorGenerator(byte audio[], Vue vue) throws UnsupportedAudioFileException {

        Complex[] resultFFT = applyFFTOnByteArrayWithoutDivide(audio); // apply FFT on our audio

        double maxMagnitude = -1;
        int index = 0; // max magnitude index
        double maxFreq = 0;
        double freq = 0;
        final double maxIntensity = 11500;
        double intensity;
        double sumSamples = 0;

        for (int i = 0; i < audio.length; i++)
            sumSamples += audio[i] * audio[i];

        intensity = (sumSamples / (audio.length)) * 100 / maxIntensity;
        intensity = roundDouble(intensity);

        for (int i = 0; i < (resultFFT.length); i++) {
            Complex complex = resultFFT[i];
            double magnitude = Math.sqrt(complex.re() * complex.re() + complex.im() * complex.im());

            // ignore low sound
            if (magnitude < Vue.getSliderValue())
                continue;

            // keep highest magnitude
            // and its corresponding frequency if between 40 Hz and 500 Hz
            if (magnitude > maxMagnitude) {
                freq = (i * sampleRate) / resultFFT.length;
                if (freq > 500 || freq < 40)
                    continue;

                maxMagnitude = magnitude;
                index = i;
                maxFreq = freq;
                // System.out.println("update magnitude :" + index);
                // System.out.println(freq);
                // System.out.println(freqs[i][index] + " index :"+index);
            }
        }
        /*
         * System.out.println("result length :" + resultFFT.length);
         * System.out.println("max magnitude :" +maxMagnitude);
         * System.out.println("max magnitude index :" +index);
         * System.out.println("result[i] : " +resultFFT[index]+ "\n\n");
         */

        Color result;

        // index of color inside array colors[1020]
        int i = (int) ((maxFreq - 40) * 2.3);

        // put in result appropriate color
        if (i >= 0 && i < color.length - 1)
            result = color[i];
        else if (maxFreq <= 0) // default color low frequency
            result = color[0];
        else // default color high frequency
            result = color[color.length - 1];

        // set music panel
        vue.setMusicRGB(result.getRed(), result.getGreen(), result.getBlue());
        vue.setMusicIntensity(intensity);

        if (arduino != null && isArduinoHere)
            arduino.writeData(colorToStringColorRGB(result));

        return result;

    }


    /**
     * generate a fingerprint as a double
     * @param freq1
     * @param freq2
     * @param freq3
     * @param freq4
     * @param freq5
     * @return double
     */
    private static double fingerPrint(double freq1, double freq2,
            double freq3, double freq4, double freq5) {
        return 100000000000. * freq5 + 100000000 * freq4 + 100000 * freq3 + 100 * freq2 + freq1;
    }


    /**
     * function test
     * read bensound-ukulele.mp3 and create its firsts fingerprints
     *
     * @throws IOException
     */
    public static void testReadFileAndFingerPrint() throws IOException {
        InputStream inputstream;
        inputstream = new FileInputStream("res/music/soft/bensound-ukulele.mp3");
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        byte[] buffer = new byte[44100];
        int bytesRead;
        int max = 0;

        // fingerprints
        HashMap<Double, Integer> fps = new HashMap<>();

        int seacher3 = 0;
        while ((bytesRead = inputstream.read(buffer)) > 0) {
            baos.write(buffer, 0, bytesRead);
            byte audio[] = baos.toByteArray();
            // Calcul.fingerPrintGenerator2(audio); //call the appropriate generator to test
            // max = Calcul.fingerPrintSeacher(audio, null, max);
            // max = Calcul.fingerPrintSeacher2(audio, null, max, fps );
            seacher3 = Calcul.fingerPrintSeacher3(audio, null);
            // System.out.println(seacher3+"\n");;
            if (seacher3 == 1)
                break;
        }
        inputstream.close();
    }

    /**
     * generate a fingerprint due to a music
     *
     * @return double[]
     */
    public static double[] fingerPrintGenerator2(byte audio[]) {

        Complex[][] resultFFT = applyFFTOnByteArray(audio); // apply FFT on our audio
        double fingerPrintArray[] = new double[resultFFT.length];

        // initialize maxMagnitude, the peak dB at each moment
        double maxMagnitude[][] = new double[resultFFT.length][range.length]; // our peak dB
        for (int i = 0; i < resultFFT.length; i++) {
            for (int j = 0; j < range.length; j++)
                maxMagnitude[i][j] = 0;
        }

        // initialize frequencies
        // save most important frequencies of each range of each sample
        double freqs[][] = new double[resultFFT.length][range.length]; // our important saved frequency
        for (int i = 0; i < resultFFT.length; i++) {
            for (int j = 0; j < range.length; j++)
                freqs[i][j] = 0;
        }

        // freq = (i * sampleRate)/resultFFT.length;
        // index we need to know to have frequencies between 40-300 Hz
        int min = 40 * resultFFT[0].length / sampleRate; // calculate index for a min frequency 40 hz
        int max = 300 * resultFFT[0].length / sampleRate; // calculate index for a max frequency 300 hz


        for (int i = 0; i < resultFFT.length; i++) {
            for (int freq = min; freq < max; freq++) { // index min 40hz to max 300hz
                // double magnitude = resultFFT[i][freq].abs(); //magnitude

                // get magnitude
                Complex complex = resultFFT[i][freq];
                double magnitude = Math.sqrt(complex.re() * complex.re() + complex.im() * complex.im());

                // ignore low sound
                if (magnitude < Vue.getSliderValue())
                    continue;

                // get range of the frequency
                int frequency = (freq * sampleRate) / resultFFT[i].length;
                int index = getIndexRange(frequency);

                // keep highest magnitude
                // and its corresponding frequency
                if (magnitude > maxMagnitude[i][index]) {
                    maxMagnitude[i][index] = magnitude;
                    freqs[i][index] = frequency; // frequency of the max dB
                }

            }

            // display test
            /*
             * for (int k = 0; k < range.length; k++)
             * System.out.print(freqs[i][k]+ " ");
             * System.out.println("");
             * // System.out.println(freqs[i][0]);
             */

            // form fingerPrint
            fingerPrintArray[i] = fingerPrint(freqs[i][0], freqs[i][1],
                    freqs[i][2], freqs[i][3], freqs[i][4]);
        }

        /*
         * for(int i = 0; i<fingerPrintArray.length; i++)
         * System.out.println("finger print : " + fingerPrintArray[i]);
         */

        return fingerPrintArray;
    }

    // statement for sql query
    public static Statement stmt = DBFiller.initConnection();


    private static String musicType = "";

    /**
     * method 3 to search a fingerprint and find appropriate genre
     * get the uniq music that used partialy the generated fingerprints
     *
     * @param audio
     * @param vue
     * @return id of the matched music in db
     */
    public static int fingerPrintSeacher3(byte audio[], Vue vue) {
        String genre = "";
        String sql = ""; // future sql query
        ResultSet res; // res of an sql command
        int id = 0; // id found by sql query

        // generate fingerprints
        double fingerPrintArray[] = Calcul.fingerPrintGenerator2(audio); // fft

        // find a music that used all or, a part of the generated fingerprints
        sql = "select music_id FROM musics_fingerPr WHERE fingerPr IN (";

        for (int i = 0; i < fingerPrintArray.length - 1; i++) {
            sql = sql + fingerPrintArray[i] + ", ";
        }
        sql = sql + fingerPrintArray[fingerPrintArray.length - 1];

        // match exactly 85% of our generated fingerprints
        sql = sql + ") GROUP BY music_id having count(distinct musics_fingerPr) >= "
                + fingerPrintArray.length * 90 / 100 +
                " ORDER BY count(distinct musics_fingerPr) DESC;";

        // execute sql query
        try {
            res = stmt.executeQuery(sql);
            if (res.next()) { // if we got a result
                id = res.getInt("music_id");

                // no uniq match, we want only uniq match
                if (res.next())
                    return -1;
                res.close();

            // no match
            } else
                return -1;

            // get the genre of the matched music id
            sql = "select genre FROM classification WHERE music_id = " + id + ";";
            res = stmt.executeQuery(sql);
            if (res.next()) {
                genre = res.getString("genre");
                res.close();
                setMessage(genre);
            }
            return id;
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return -1;
    }

    // property change support, use it as an observer for value musicType
    private final static PropertyChangeSupport support = new PropertyChangeSupport(Calcul.class);

    /**
     * add listener in support
     *
     * @param listener
     */
    public static void addPropertyChangeListener(PropertyChangeListener listener) {
        support.addPropertyChangeListener(listener);
    }

    /**
     * remove listener in support
     * @param listener
     */
    public static void removePropertyChangeListener(PropertyChangeListener listener) {
        support.removePropertyChangeListener(listener);
    }

    /**
     * set message, string contained in attribute support(PropertyChangeListener)
     * in our situation, only musicType is set
     * @param newValue
     */
    public static void setMessage(String newValue) {
        String oldValue = musicType;
        musicType = newValue;
        // The parameter values of firePropertyChange method
        // constitute the PropertyChangeEvent object
        support.firePropertyChange("musicType", oldValue, newValue);
    }

    /**
     * musicType getter
     * @return String
     */
    public static String getMusicType() {
        return musicType;
    }


    /**
     * return a array of 1020 Color (from blue to red)
     * use it to initialize static color
     * @returnColor[]
     */
    public static Color[] initColor() {

        // if color already filled
        if (color != null)
            return null;

        // initialization
        Color[] colors = new Color[1020]; // range of 1020 colors
        colors[0] = new Color(0, 0, 255); // 50,50,205 0,255,0

        for (int i = 1; i < 1020; i++) {

            /**
             * fill colors with a graduation of 1020 colors from blue to red
             */
            if (colors[i - 1].getBlue() == 255 && colors[i - 1].getGreen() < 255)
                colors[i] = new Color(colors[i - 1].getRed(),
                        colors[i - 1].getGreen() + 1, colors[i - 1].getBlue());

            else if (colors[i - 1].getRed() == 255 && colors[i - 1].getGreen() > 0)
                colors[i] = new Color(colors[i - 1].getRed(),
                        colors[i - 1].getGreen() - 1, colors[i - 1].getBlue());

            else if (colors[i - 1].getGreen() == 255 && colors[i - 1].getBlue() > 0)
                colors[i] = new Color(colors[i - 1].getRed(),
                        colors[i - 1].getGreen(), colors[i - 1].getBlue() - 1);

            else if (colors[i - 1].getGreen() == 255 && colors[i - 1].getRed() < 255)
                colors[i] = new Color(colors[i - 1].getRed() + 1,
                        colors[i - 1].getGreen(), colors[i - 1].getBlue());

        }
        return colors;
    }

    /**
     * convert a color to a string, seperated with commas
     *  and end with a line break
     * @param color
     * @return
     */
    public static String colorToStringColorRGB(Color color) {
        return color.getRed() + ", "+ color.getGreen() +
         " " + color.getBlue() +"\n";
    }

    /**
     * Rounds a double value to 2 decimals
     * @param val
     * @return
     */
    public static double roundDouble(double val) {
        BigDecimal bd = BigDecimal.valueOf(val);
        bd = bd.setScale(2, RoundingMode.HALF_UP);

        return bd.doubleValue();
    }
}
