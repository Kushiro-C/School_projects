import java.io.*;
public class Parser {

    /**
     * Private Constructor
     */
    private Parser() {}

    /**
     * This function reads from the buffered reader and
     * returns a String ending with "\r\n"
     *
     * @param br BufferedReader
     * @return String
     */
    public static String readFullLine(BufferedReader br) {
        String mess = "";
        char cr = '\0'; // Init Carriage Return
        char c; // Init c
        try {
            // A Message MUST END with "\r\n" as said in subject
            while (((c = (char) br.read()) != '\n') || (cr != '\r')) {
                if (!br.ready() && c != '\n' && cr != '\r')
                    throw new IOException("Message did not end with \"\\r\\n\"");
                cr = c;
                mess += c;
            }
            mess += c;
        } catch (IOException e) {
            System.out.println(e.getMessage());
            return "";
        }
        return mess;
    }

    /**
     * Checks if the string is a valid message for DIFF
     * [DIFF␣num-mess␣id␣message] 161 bytes
     * @param str
     * @return boolean
     */
    public static boolean checkDIFF(String str) {
        // Check spaces
        if (str.length() != 161 || str.charAt(9) != ' ' || str.charAt(18) != ' ')
            return false;
        // Check keywords
        if (!str.startsWith("DIFF ") || !str.endsWith("\r\n"))
            return false;
        try {
            Formatter.unFormatNumber(str.substring(5, 9)); // Check num-mess
        } catch (Exception e) {
            return false;
        }
        return true;
    }

    /**
     * Checks if the string is a valid message for REGI
     * [REGI␣id␣ip1␣port1␣ip2␣port2] 57 bytes
     *
     * @param str
     * @return boolean
     */
    public static boolean checkREGI(String str) {
        // Check spaces
        if (str.length() != 57 || str.charAt(13) != ' ' || str.charAt(29) != ' ' ||
            str.charAt(34) != ' ' || str.charAt(50) != ' ')
            return false;
        // Check keywords
        if (!str.startsWith("REGI ") || !str.endsWith("\r\n"))
            return false;
        String tab[] = str.substring(5, 55).split(" ");
        if (tab.length != 5 || tab[0].length() != 8 || tab[1].length() != 15 ||
            tab[2].length() != 4 || tab[3].length() != 15 || tab[4].length() != 4)
            return false;
        try {
            // Check IPs
            if (!Formatter.formatIPv4(tab[1]).equals(tab[1]) ||
                !Formatter.formatIPv4(tab[3]).equals(tab[3])) {
                    System.out.println("FAIL TRY");
                    return false;
                }
            // Check Ports
            Formatter.unFormatNumber(tab[2]);
            Formatter.unFormatNumber(tab[4]);
        } catch (Exception e) {
            return false;
        }
        return true;
    }

    /**
     * Checks if the string is a valid message for MESS
     * [MESS␣id␣message] 156 bytes
     *
     * @param str
     * @return boolean
     */
    public static boolean checkMESS(String str) {
        // Check spaces
        if (str.length() != 156 || str.charAt(13) != ' ')
            return false;
        // Check keywords
        if (!str.startsWith("MESS ") || !str.endsWith("\r\n"))
            return false;
        return true;
    }

    /**
     * Checks if the string is a valid message for LAST
     * [LAST␣nb-mess] 10 bytes
     *
     * @param str
     * @return boolean
     */
    public static boolean checkLAST(String str) {
        // Check keywords
        if (str.length() != 10 || !str.startsWith("LAST ") || !str.endsWith("\r\n"))
            return false;
        try {
            Formatter.unFormatNumber(str.substring(5, 8));
        } catch (Exception e) {
            return false;
        }
        return true;
    }

    // Test purposes (Simple tests functions above)
    public static void main(String[] args) {
        // Init Messages Values
        String mess = "";
        for (int i = 0; i < 140; i++)
            mess += 'a';
        String id = "RADIO###";
        String ip1 = "127.000.000.001";
        String ip2 = "225.001.002.004";
        String num_mess = "0012";
        String nb_mess = "098";
        String port1 = "5551";
        String port2 = "5552";

        String diffOK = "DIFF " + num_mess + " " + id + " " + mess + "\r\n";
        String regiOK = "REGI " + id + " " + ip1 + " " + port1 + " " + ip2 + " " + port2 + "\r\n";
        String messOK = "MESS " + id + " " + mess + "\r\n";
        String messOK2 =
            "MESS channele Sed ut perspiciatis unde omnis iste natus error sit " +
            "voluptatem accusantium dol1########################################" +
            "#####################\r\n";
        String lastOK = "LAST " + nb_mess + "\r\n";

        System.out.println("Test DIFF OK " + checkDIFF(diffOK));
        System.out.println("Test REGI OK " + checkREGI(regiOK));
        System.out.println("Test MESS OK " + checkMESS(messOK));
        System.out.println("Test MESS OK2 " + checkMESS(messOK2));
        System.out.println("Test LAST OK " + checkLAST(lastOK));

        String diffNO = "DIFF " +  "0a23 " + id + " " + mess + "\r\n"; // num_mess not number
        String regiNO = "REGI" + id + " " + ip1 + " " + port1 + " " + ip2 + " " + port2 + "\n"; // Missing end
        String messNO = "MESS " + id + " " + mess + "\r\n "; // Extra Space at the end
        String lastNO = "LAST " + nb_mess + "  \n"; // Space instead of carriage return

        System.out.println("\nTest DIFF NO " + checkDIFF(diffNO));
        System.out.println("Test REGI NO " + checkREGI(regiNO));
        System.out.println("Test MESS NO " + checkMESS(messNO));
        System.out.println("Test LAST NO " + checkLAST(lastNO));
    }
}