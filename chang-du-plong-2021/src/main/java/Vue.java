import java.awt.GridLayout;
import java.awt.Color;
import java.awt.Font;
import java.awt.*;
import javax.swing.*;
import javax.swing.border.EmptyBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

/**
 * Simple GUI with a window split in half
 * Each Panel has a background color representing
 * the color of an Ampoule, and a Text (JLabel)
 * with the Intensity value.
 */
public class Vue {
    private Ampoule ampMusic; // Ampoule for left JPanel
    private Ampoule ampVoice; // Ampoule for right JPanel

    private JFrame jFrame;
    private JPanel container; // Contains left and right Panels
    private JPanel bigContainer; // Contains all Panels
    private JPanel leftPanel;
    private JPanel rightPanel;

    private JLabel intensityLeft;
    private JLabel intensityRight;
    private JLabel minMagnitude;
    private JLabel musicType;

    private GridBagConstraints c;

    private JSlider slider;
    private JPanel sliderPanel;
    private static int sliderValue = 0;


    /**
     * Constructor Vue basic GUI
     * It is a very simple display
     * @param ampMusic Ampoule
     * @param ampVoice Ampoule
     */
    Vue(Ampoule ampMusic, Ampoule ampVoice) {
        this.ampMusic = ampMusic; // Set Ampoule Music
        this.ampVoice = ampVoice; // Set Ampoule Voice

        // Init Components
        jFrame = new JFrame(); // Create instance JFrame
        container = new JPanel(); // Contains left and right JPanels
        bigContainer = new JPanel(); // Contains all JPanels
        leftPanel = new JPanel(); // Left JPanel for ampMusic
        rightPanel = new JPanel(); // Right JPanel for ampVoice
        slider = new JSlider(0, 10000, 0);
        sliderPanel = new JPanel();
        c = new GridBagConstraints(); //constraints
        // Text Left Panel
        intensityLeft = new JLabel(
            "Intensité : " + String.valueOf(ampMusic.getIntensity()));
        // Text Right Panel
        intensityRight = new JLabel(
            "Intensité : " + String.valueOf(ampVoice.getIntensity()));
        // Text magnitude value
        minMagnitude = new JLabel(
            "La valeur minimum de la magnitude est : " + slider.getValue());
        musicType = new JLabel(
            "La musique se rapproche du : " + Calcul.getMusicType());

        // Set Background color of JPanels
        leftPanel.setBackground(
            new Color(ampMusic.getR(), ampMusic.getG(), ampMusic.getB()));
        rightPanel.setBackground(
            new Color(ampVoice.getR(), ampVoice.getG(), ampVoice.getB()));

        // Set JLabels
        intensityLeft.setForeground(Color.WHITE); // Text Color
        intensityLeft.setBackground(Color.GRAY); // Background
        intensityLeft.setOpaque(true); // Show BG
        intensityLeft.setFont(new Font("Montserrat", Font.BOLD, 24)); // Font

        intensityRight.setForeground(Color.WHITE); // Text Color
        intensityRight.setBackground(Color.GRAY); // Background
        intensityRight.setOpaque(true); // Show BG
        intensityRight.setFont(new Font("Montserrat", Font.BOLD, 24)); // Font

        // Add Text to JPanels
        leftPanel.add(intensityLeft);
        rightPanel.add(intensityRight);

        // Set Container Layout (Left | Right)
        container.setLayout(new GridLayout(1, 2));

        // Add slider to sliderPanel
        sliderPanel.setLayout(new GridLayout(3, 1));
        sliderPanel.setBorder(new EmptyBorder(10, 10, 10, 10));
        minMagnitude.setHorizontalAlignment(JLabel.CENTER);
        musicType.setHorizontalAlignment(JLabel.CENTER);
        sliderPanel.add(musicType);
        sliderPanel.add(minMagnitude);
        sliderPanel.add(slider);
        slider.setPaintTrack(true);
        slider.setPaintTicks(true);
        slider.setPaintLabels(true);
        slider.setMajorTickSpacing(2000);
        slider.setMinorTickSpacing(1000);

        // Add sliderPanel to bigContainer
        bigContainer.setLayout(new GridBagLayout());
        c.fill = GridBagConstraints.HORIZONTAL;
        c.gridx = 0;
        c.gridy = 1;
        c.gridwidth = 2;
        bigContainer.add(sliderPanel, c);

        // Add container to bigContainer
        container.add(leftPanel);
        container.add(rightPanel);
        c.fill = GridBagConstraints.HORIZONTAL;
        c.ipady = 580;      //make this component tall
        c.weightx = 1;
        c.gridwidth = 2;
        c.weighty = 0.8;
        c.gridx = 0;
        c.gridy = 0;
        bigContainer.add(container, c);

        // Add JPanels to the JFrame
        jFrame.add(bigContainer);

        // JFrame settings
        jFrame.setTitle("Ampoule GUI");
        jFrame.setSize(1000, 600); // 1000 width, 600 height
        jFrame.setLocation(0, 0);
        jFrame.setVisible(true); // making the JFrame visible
        jFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        //slider Listener
        slider.addChangeListener(new ChangeListener(){
            @Override
            public void stateChanged(ChangeEvent e) {
                sliderValue = slider.getValue();
                minMagnitude.setText("La valeur minimum de la magnitude est : " + slider.getValue());
            }
        });

        /**
         * Calcul class musicType observer
         * update musicType JLabel to Calcul class's musicType value
         */
        Calcul.addPropertyChangeListener(e ->
            musicType.setText("La musique se rapproche du : " +(String) e.getNewValue())
        );
    }

    /**
     * Sets Ampoule RGB for Music (Left) and update view
     */
    public void setMusicRGB(int r, int g, int b) {
        ampMusic.setR(r);
        ampMusic.setG(g);
        ampMusic.setB(b);

        leftPanel.setBackground(
            new Color(ampMusic.getR(), ampMusic.getG(), ampMusic.getB()));
    }

    /**
     * Sets Ampoule RGB for Voice (Right) and update view
     */
    public void setVoiceRGB(int r, int g, int b) {
        ampVoice.setR(r);
        ampVoice.setG(g);
        ampVoice.setB(b);
        rightPanel.setBackground(
            new Color(ampVoice.getR(), ampVoice.getG(), ampVoice.getB()));
    }

    /**
     * Sets Ampoule Intensity for Music (Left) and update view
     */
    public void setMusicIntensity(double intensity) {
        ampMusic.setIntensity(intensity);
        intensityLeft.setText("Intensité : " +
            String.valueOf(ampMusic.getIntensity()));
    }

    /**
     * Sets Ampoule Intensity for Voice (Right) and update view
     */
    public void setVoiceIntensity(double intensity) {
        ampVoice.setIntensity(intensity);
        intensityRight.setText("Intensité : " +
            String.valueOf(ampVoice.getIntensity()));
    }

    /**
     * sliderValue getter
     * @return int
     */
    public static int getSliderValue() {
        return sliderValue;
    }
}
