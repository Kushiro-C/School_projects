import org.knowm.xchart.SwingWrapper;
import org.knowm.xchart.XYChart;
import org.knowm.xchart.XYChartBuilder;
import org.knowm.xchart.style.markers.SeriesMarkers;

/**
 * Class creating a Realtime graph
 * to display frequencies
 */
public class Graph {
    private final XYChart chart;
    private final SwingWrapper<XYChart> sw;

    /**
     * Graph constructor creates the chart and displays it
     */
    public Graph() {
        // Create the chart
        chart = new XYChartBuilder()
            .width(1100).height(600)
            .title("Frequency Graph Over Time")
            .xAxisTitle("Relative Time")
            .yAxisTitle("Frequency")
            .build();

        // Add 'frequency' in the chart
        chart.addSeries("frequency", new double[] {0})
            .setMarker(SeriesMarkers.NONE); // Style

        // Display the chart
        sw = new SwingWrapper<XYChart>(chart);
        sw.displayChart();
    }

    /**
     * Updates the graph with the values
     */
    public void updateGraph(double[] freq) {
        final double[][] data = getFreqData(freq);

        chart.updateXYSeries("frequency", data[0], data[1], null);
        sw.repaintChart();
    }

    /**
     * Prepares the data from the frequencies
     * for the chart to repaint
     * @param freq
     * @return
     */
    private static double[][] getFreqData(double[] freq) {
        double[] xData = new double[freq.length];
        double[] yData = new double[freq.length];

        for (int i = 0; i < freq.length; i++) {
            xData[i] = i;
            yData[i] = freq[i];
        }

        return new double[][] {xData, yData};
    }
}
