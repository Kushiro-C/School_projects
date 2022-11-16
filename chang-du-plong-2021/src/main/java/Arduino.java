import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStream;
import gnu.io.CommPortIdentifier;
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent;
import gnu.io.SerialPortEventListener;
import java.util.Enumeration;

public class Arduino implements SerialPortEventListener {
	SerialPort serialPort;
	/** The port we're normally going to use. */
	private static final String PORT_NAMES[] = {
			"/dev/ttyACM0", // Linux
            "/dev/tty.usbserial-A9007UX1", // Mac OS X
            "/dev/ttyUSB0", // also Linux
            "COM4", // Windows
	};
	/**
	 * A BufferedReader which will be fed by a InputStreamReader
	 * converting the bytes into characters
	 * making the displayed results codepage independent
	 */
	private BufferedReader input;
	/** The output stream to the port */
	private OutputStream output;

	int cpt = 0;

	public OutputStream getOutput() {
		return output;
	}

	public void setOutput(OutputStream output) {
		this.output = output;
	}

	/** Milliseconds to block while waiting for port open */
	private static final int TIME_OUT = 2000;
	/** Default bits per second for COM port. */
	private static final int DATA_RATE = 9600;

	public boolean initialize() {
		// the next line is for Raspberry Pi and
		// gets us into the while loop and was suggested here was suggested
		// https://www.raspberrypi.org/phpBB3/viewtopic.php?f=81&t=32186
		System.setProperty("gnu.io.rxtx.SerialPorts", "/dev/ttyACM0");

		CommPortIdentifier portId = null;
		Enumeration portEnum = CommPortIdentifier.getPortIdentifiers();

		// First, Find an instance of serial port as set in PORT_NAMES.
		while (portEnum.hasMoreElements()) {
			CommPortIdentifier currPortId = (CommPortIdentifier) portEnum.nextElement();
			for (String portName : PORT_NAMES) {
				if (currPortId.getName().equals(portName)) {
					portId = currPortId;
					break;
				}
			}
		}
		if (portId == null) {
			System.out.println("Could not find COM port.");
			return false;
		}

		try {
			// open serial port, and use class name for the appName.
			serialPort = (SerialPort) portId.open(this.getClass().getName(),
					TIME_OUT);

			// set port parameters
			serialPort.setSerialPortParams(DATA_RATE,
					SerialPort.DATABITS_8,
					SerialPort.STOPBITS_1,
					SerialPort.PARITY_NONE);

			// open the streams
			input = new BufferedReader(new InputStreamReader(serialPort.getInputStream()));
			output = serialPort.getOutputStream();

			// add event listeners
			serialPort.addEventListener(this);
			serialPort.notifyOnDataAvailable(true);
            return true;
		} catch (Exception e) {
			System.err.println(e.toString());
		}
        return false;
	}

	/**
	 * This should be called when you stop using the port.
	 * This will prevent port locking on platforms like Linux.
	 */
	public synchronized void close() {
		if (serialPort != null) {
			serialPort.removeEventListener();
			serialPort.close();
		}
	}

	/**
	 * Handle an event on the serial port. Read the data and print it.
	 */
	public synchronized void serialEvent(SerialPortEvent oEvent) {
		if (oEvent.getEventType() == SerialPortEvent.DATA_AVAILABLE) {
			try {
				String inputLine = input.readLine();
				System.out.println(inputLine);
			} catch (Exception e) {
				System.err.println(e.toString());
			}
		}
		// Ignore all the other eventTypes, but you should consider the other ones.
	}

	// This simply sends the data
	public synchronized void writeData(String data) {
		System.out.println("Sent: " + data);
		try {
			/*if(cpt >= 2) {
				cpt = 0;
				output.flush();
			} else 
				cpt ++;*/
			output.write(data.getBytes());
		} catch (Exception e) {
			System.out.println("could not write to port");
		}
	}

	public static void main(String[] args) throws Exception {
		Arduino main = new Arduino();
		main.initialize();

		Thread t = new Thread() {
			public void run() {
				// the following line will keep this app alive for 1000 seconds,
				// waiting for events to occur and responding to them (printing incoming
				// messages to console).
				try {
					sleep(1);
					main.writeData("242, 12, 189\n"); //pink

					//sleep(1000);
					main.writeData("252, 190, 3"); //yellow-orange
					/*main.writeData("242\n");
					main.writeData("12\n");
					main.writeData("189\n");
					sleep(1000);
					main.writeData("252\n");
					main.writeData("190\n");
					main.writeData("3\n");*/

					/*main.writeData("190\n");
					main.writeData("3\n");*/
					/*main.writeData("255");
					main.writeData("50");
					main.writeData("50");
					sleep(4000);
					main.writeData("100");
					main.writeData("100");
					main.writeData("50");
					Thread.sleep(1000000);*/
				} catch (InterruptedException ie) {
				}
			}
		};
		t.start();
		System.out.println("Started");
	}
}
