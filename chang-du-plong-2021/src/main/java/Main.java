import java.io.*;

import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.UnsupportedAudioFileException;

public class Main {
    public static void main(String[] args) {
        // Basic light bulbs
        Ampoule ampouleMusic = new Ampoule(42, 42, 42, 0);
        Ampoule ampouleVoice = new Ampoule(42, 42, 42, 0);

        try {
            // It is already filled = 1
            if (DBFiller.fillDataBase() == 1) {
                // Open Basic GUI
                Vue vue = new Vue(ampouleMusic, ampouleVoice);
                Graph graph = new Graph();

                Recorder.startRecord(vue, graph);
            }

            //Calcul.testReadFileAndFingerPrint();
        } catch (Exception e) {
            System.err.println(e);
        }
    }
}
