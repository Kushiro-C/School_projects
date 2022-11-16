import java.util.Objects;

import javax.swing.JOptionPane;

import org.jtransforms.fft.DoubleFFT_1D;

import javax.sound.sampled.*;
import java.io.*;
import javax.swing.*;
import java.lang.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.awt.Color;

public class Recorder {
    // Audio Format settings
    private final static int sampleRate = 44100; // Hz
    private final static int sampleSize = 16; // bits
    private final static int channels = 1; // 1: Mono | 2: Stereo
    private final static boolean signed = true;
    private final static boolean bigEndian = true;
    private final static int bufSize = 4096;

    /**
     * Creates and runs multiple threads for the panels
     * @param vue
     */
    public static void startRecord(Vue vue, Graph graph) {
        // Left panel for Music
        Thread freqColorThread = new Thread("freqColor") {
            public void run() {
                freqRecord(vue, graph);
            }
        };

        // Database [soft, medium, hard] classification
        Thread classsificationThread = new Thread("classificationColor") {
            public void run() {
                classificationRecord(vue);
            }
        };

        // For test purposes : replays microphone audio to speakers
        Thread replayThread = new Thread("replaySound") {
            public void run() {
                try {
                    replaySoundToSpeaker();
                } catch (UnsupportedAudioFileException e) {
                    e.printStackTrace();
                }
            }
        };

        // Right panel for voice activity
        Thread voiceThread = new Thread("voice") {
            public void run() {
                try {
                    voiceActivityRecord(vue);
                } catch (UnsupportedAudioFileException e) {
                    e.printStackTrace();
                }
            }
        };

        // Realtime graph showing frequency
        Thread graphThread = new Thread("graph") {
            public void run() {
                try {
                    freqToGraph(graph);
                } catch (UnsupportedAudioFileException e) {
                    e.printStackTrace();
                }
            }
        };

        freqColorThread.start(); // Left Panel
        classsificationThread.start();
        // replayThread.start(); // For test purposes
        voiceThread.start(); // Right panel
        graphThread.start(); // Frequency Chart
    }


    /**
     * Converts an audio byte array data into a double array
     * @param audio
     * @return double[] audio
     * @throws UnsupportedAudioFileException
     */
    public static double[] byteArrayToDoubleArrayAudio(byte[] audio) throws UnsupportedAudioFileException {
        ByteBuffer bb = ByteBuffer.wrap(audio);
        bb.order(bigEndian ? ByteOrder.BIG_ENDIAN : ByteOrder.LITTLE_ENDIAN);

        // Convert byte[] to double[] (double = 8 bytes)
        double[] samples = new double[audio.length * 8 / sampleSize];

        for (int i = 0; i < samples.length; i++) {
            // Get correct data type from sampleSize
            switch (sampleSize) {
                case 8:  samples[i] = bb.get();
                         break;
                case 16: samples[i] = bb.getShort();
                         break;
                case 32: samples[i] = bb.getInt();
                         break;
                case 64: samples[i] = bb.getLong();
                         break;
                default: throw new UnsupportedAudioFileException();
            }
        }

        return samples;
    }

    /**
     * Filters the audio, removing the background noise
     * And detects voice activity
     * @param audio byte[]
     * @return byte[] audio with the background noise filtered out
     * @throws UnsupportedAudioFileException
     */
    public static byte[] handleAudioTest(byte[] audio) throws UnsupportedAudioFileException {
        ByteBuffer bb = ByteBuffer.wrap(audio);
        bb.order(bigEndian ? ByteOrder.BIG_ENDIAN : ByteOrder.LITTLE_ENDIAN);

        // Convert byte[] to double[] (double = 8 bytes)
        double[] samples = new double[audio.length * 8 / sampleSize];
        double peak = 0;

        for (int i = 0; i < samples.length; i++) {
            // Get correct data type from sampleSize
            switch (sampleSize) {
                case 8:  samples[i] = bb.get();
                         break;
                case 16: samples[i] = bb.getShort();
                         break;
                case 32: samples[i] = bb.getInt();
                         break;
                case 64: samples[i] = bb.getLong();
                         break;
                default: throw new UnsupportedAudioFileException();
            }
            peak = Math.abs(samples[i]) > peak ? Math.abs(samples[i]) : peak;
        }

        for (int i = 0; i < samples.length; i++) {
            // Filters most BG buzzing noise
            samples[i] = Calcul.normalize(samples[i], peak);
        }

        // Test Zero Crossing Rate
        double zcr = Calcul.zeroCrossingRate(samples);
        boolean voiceZCR = zcr < 170 && zcr > 15;

        // Test detect when we have a voice with ZCR
        if (voiceZCR) // Vocal range
            System.out.println("Voiced ZCR");
        else
            System.out.println("Unvoiced ZCR");

    /*
        From some tests, piano only has a ZCR close to human (male at least)
        ZCR range, so this method will detect piano solo as Voice
    */

        // Apply fft to doubleData
        DoubleFFT_1D fft_1d = new DoubleFFT_1D(samples.length);
        double[] fft_out = new double[samples.length * 2];
        System.arraycopy(samples, 0, fft_out, 0, samples.length);
        fft_1d.realForwardFull(fft_out);

        // Get all magnitudes over the threshold
        double threshold = 70000; // Testing value
        double magnitudes[] = Calcul.getAllMagnitudes(fft_out);
        int counter = Calcul.getNbAboveThreshold(magnitudes, threshold);

        // If a certain number of magnitudes are above the threshold
        // We may consider it as a voice
        System.out.println(counter);
        if (counter > 50) // Testing counter for voice detection
            System.out.println("Voiced CPT");
        else
            System.out.println("Unvoiced CPT");

        if (counter > 50 && voiceZCR)
            System.out.println("Voiced FINAL");
        else
            System.out.println("Unvoiced final");
        System.out.println();

        // Get back data (Inverse FFT)
        fft_1d.complexInverse(fft_out, true);

        // Convert double array (fft_out) into byte array
        int nbBytes = sampleSize / 8;
        for (int i = 0; i < fft_out.length / 2; i++) {
            bb = ByteBuffer.wrap(audio, i * nbBytes, nbBytes);
            switch (sampleSize) {
                case 8:  bb.put((byte) fft_out[i*2]);
                         break;
                case 16: bb.putShort((short) fft_out[i*2]);
                         break;
                case 32: bb.putInt((int) fft_out[i*2]);
                         break;
                case 64: bb.putLong((long) fft_out[i*2]);
                         break;
                default: throw new UnsupportedAudioFileException();
            }
        }

        return audio;
    }

    /**
     * This function is for test purposes
     * Replays the sound from the microphone into the speakers
     * It is used to test what the computer heard
     * @throws UnsupportedAudioFileException
     */
    public static void replaySoundToSpeaker() throws UnsupportedAudioFileException {
        // Create Audio Format
        AudioFormat format = new AudioFormat(
            sampleRate, sampleSize, channels, signed, bigEndian);

        // Prepare Target and Source Line Info for Microphone and Speakers
        DataLine.Info targetLineInfo = new DataLine.Info(TargetDataLine.class, format);
        DataLine.Info sourceLineInfo = new DataLine.Info(SourceDataLine.class, format);

        try {
            // Get Microphone and Speakers from Line Infos
            TargetDataLine microphone = (TargetDataLine) AudioSystem.getLine(targetLineInfo);
            SourceDataLine speakers = (SourceDataLine) AudioSystem.getLine(sourceLineInfo);

            // Open and start Microphone and Speakers
            microphone.open(format);
            microphone.start();
            speakers.open(format);
            speakers.start();

            // Bytes Buffer
            byte[] data = new byte[bufSize];
            boolean running = true;

            // Always true for now
            while (running) {
                // Read from TargetDataLine into ByteBuffer
                int count = microphone.read(data, 0, data.length);

                // Number of bytes microphone could read
                if (count > 0) {
                    // Filters audio and voice detection
                    data = handleAudioTest(data);

                    // Write ByteBuffer into SourceDataLine
                    speakers.write(data, 0, count);
                }
            }
            microphone.close();
            speakers.drain();
            speakers.close();

        } catch (LineUnavailableException e) {
            e.printStackTrace();
        }
    }

    /**
     * Changes the right panel, which is used for the voice activity
     * @param vue
     * @throws UnsupportedAudioFileException
     */
    public static void voiceActivityRecord(Vue vue) throws UnsupportedAudioFileException {
        // Create Audio Format
        AudioFormat format = new AudioFormat(
            sampleRate, sampleSize, channels, signed, bigEndian);

        // Prepare Target and Source Line Info for Microphone and Speakers
        DataLine.Info targetLineInfo = new DataLine.Info(TargetDataLine.class, format);

        if (!AudioSystem.isLineSupported(targetLineInfo)) {
            System.out.println("error: not supported !");
            return;
        }

        TargetDataLine microphone;

        try {
            // Get Microphone and Speakers from Line Infos
            microphone = (TargetDataLine) AudioSystem.getLine(targetLineInfo);

            // Open and start Microphone and Speakers
            microphone.open(format);
            microphone.start();

            // Bytes Buffer
            byte[] data = new byte[microphone.getBufferSize()/5];
            boolean running = true;

            // Always true for now
            while (running) {
                // Read from TargetDataLine into ByteBuffer
                int count = microphone.read(data, 0, data.length);

                // Number of bytes microphone could read
                if (count > 0) {
                    double[] samples = byteArrayToDoubleArrayAudio(data);
                    Calcul.voiceColorGenerator(samples, vue);
                }
            }
            microphone.close();

        } catch (LineUnavailableException e) {
            e.printStackTrace();
        }
    }

    /**
     * change vue beside frequency
     * @param vue
     */
    public static void freqRecord(Vue vue, Graph graph) {
        // recording
        AudioFormat format = new AudioFormat(
            sampleRate, sampleSize, channels, signed, bigEndian); // audio format with CD quality

        DataLine.Info info = new DataLine.Info(TargetDataLine.class,
                format); // format is an AudioFormat object
        if (!AudioSystem.isLineSupported(info)) {
            System.out.println("error: not supported !");
            return;
        }

        // the line from which audio data is captured
        TargetDataLine line;
        // Obtain and open the line.
        try {
            line = (TargetDataLine) AudioSystem.getLine(info);
            line.open(format);
            line.start();
        } catch (LineUnavailableException ex) {
            System.out.println("error: line not opened !");
            return;
        }

        int numBytesRead;
        byte[] data = new byte[line.getBufferSize()/5]; // 8820

        boolean stopped = false; // if stop recording

        // read line (start recording here)
        try {
            while (true) { // (cpt ++) <2 !stopped //TODO : change to stopped
                // Read the next chunk of data from the TargetDataLine.
                numBytesRead = line.read(data, 0, data.length);
                //System.out.println("Freq : number bytes read : " + numBytesRead);

                // Save data
                if (numBytesRead > 0) {
                    // real time part
                    // Update Music Panel
                    Color color = Calcul.colorGenerator(data, vue);
                }

            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * change vue beside music classification
     * @param vue
     */
    public static void classificationRecord(Vue vue) {
        // Create Audio Format
        AudioFormat format = new AudioFormat(
            sampleRate, sampleSize, channels, signed, bigEndian);

        DataLine.Info info = new DataLine.Info(TargetDataLine.class,
                format); // format is an AudioFormat object
        if (!AudioSystem.isLineSupported(info)) {
            System.out.println("error: not supported !");
            return;
        }

        // the line from which audio data is captured
        TargetDataLine line;
        // Obtain and open the line.
        try {
            line = (TargetDataLine) AudioSystem.getLine(info);
            line.open(format);
            line.start();
        } catch (LineUnavailableException ex) {
            System.out.println("error: line not opened !");
            return;
        }

        int numBytesRead;
        byte[] data = new byte[line.getBufferSize()]; // my buffer /5 ?

        boolean stopped = false; // if stop recording

        // read line (start recording here)
        int res = -1;
        try {
            while (true) { // (cpt ++) <2 !stopped //TODO : change to stopped
                // Read the next chunk of data from the TargetDataLine.
                numBytesRead = line.read(data, 0, data.length);
                //System.out.println("Classification : number bytes read : " + numBytesRead);

                //---test method 3----
                if(numBytesRead > 0 && res == -1) {
                    //System.out.println("iiin");
                    res = Calcul.fingerPrintSeacher3(data, null);
                    if(res != -1) {
                        //System.out.println(res);
                        res = -1;
                    }
                }
            }
        } catch (Exception e) {
            System.out.println("error: read line !");
        }
    }

    /**
     * Gets the realtime audio and shows it in the graph
     * @param graph
     * @throws UnsupportedAudioFileException
     */
    public static void freqToGraph(Graph graph) throws UnsupportedAudioFileException {
        // Create Audio Format
        AudioFormat format = new AudioFormat(
            sampleRate, sampleSize, channels, signed, bigEndian);

        // Prepare Target and Source Line Info for Microphone and Speakers
        DataLine.Info targetLineInfo = new DataLine.Info(TargetDataLine.class, format);

        try {
            // Get Microphone and Speakers from Line Infos
            TargetDataLine microphone = (TargetDataLine) AudioSystem.getLine(targetLineInfo);

            // Open and start Microphone and Speakers
            microphone.open(format);
            microphone.start();

            // Bytes Buffer
            byte[] data = new byte[512];
            boolean running = true;

            // Always true for now
            while (running) {
                // Read from TargetDataLine into ByteBuffer
                int count = microphone.read(data, 0, data.length);

                // Number of bytes microphone could read
                if (count > 0) {
                    // Update the graph
                    double[] dbData = byteArrayToDoubleArrayAudio(data);
                    graph.updateGraph(dbData);
                }
            }
            microphone.close();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
