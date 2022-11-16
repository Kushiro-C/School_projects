public class Formatter {

    /**
     * Private Constructor
     */
    private Formatter() {}

    /**
     * Converts a String into a byte array
     *
     * @param str String to convert
     * @return the converted byte[]
     */
    public static byte[] stringToByteArray(String str) {
        return str.getBytes();
    }

    /**
     * Converts a byte array into a String
     *
     * @param bytes array to convert
     * @return the converted String
     */
    public static String byteArrayToString(byte[] bytes) {
        return new String(bytes, 0, bytes.length);
    }

    /**
     * Formats a message into a given fixed size
     * by adding '#' at the end of the message until
     * the new message has exactly 'size' characters,
     * or cuts the message to 140 chars, if longer than size.
     * For an id (size = 8), "RADIO" becomes "RADIO###"
     *
     * @param message String message to format
     * @param size    int fixing the size of the new message
     * @return the formatted String message
     */
    public static String formatMessage(String message, int size) {
        int len = message.length();
        if (len > size)
            return message.substring(0, size);
        int i = 0;
        while (len + i < size) {
            i++;
            message += '#';
        }
        return message;
    }

    /**
     * Formats a number into a fixed size
     * by adding '0' at the beginning until
     * the new number has exactly 'size' characters
     * For num-mess (size = 4), 120 becomes "0120"
     *
     * @param number int to format
     * @param maxDigits   int max digits of the number
     * @return the formatted number as a String
     * @throws BadNumberFormatException if number is negative, or bigger than max digit
     */
    public static String formatNumber(int number, int maxDigits) throws BadNumberFormatException {
        if (number < 0 || number > Math.pow(10, maxDigits) - 1)
            throw new BadNumberFormatException(
                "Number and size don't match, number=" + number + ", maxDigits=" + maxDigits);

        String newNumber = String.valueOf(number);
        while (newNumber.length() < maxDigits)
            newNumber = '0' + newNumber;

        return newNumber;
    }

    /**
     * Formats an IPv4 Address into a fixed size of 15 chars
     * by adding '0' at the beginning of each part
     * "127.0.0.1" becomes "127.000.000.001"
     *
     * @param ip String in an IPv4 address format
     * @return the formatted IPv4 address as a String
     * @throws BadIpFormatException if it is not in a valid IPv4 address format
     */
    public static String formatIPv4(String ip) throws BadIpFormatException {
        if (!ip.matches("[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}"))
            throw new BadIpFormatException("Bad IPv4 Format, ip:" + ip);

        String newIp = "";
        String[] myIp = ip.split("\\.");
        int len = myIp.length;
        for (int i = 0; i < len; i++) {
            for (int j = myIp[i].length(); j < 3; j++)
                myIp[i] = '0' + myIp[i];
            newIp += myIp[i];
            if (i != len - 1)
                newIp += '.';
        }
        return newIp;
    }

    /**
     * Unformats a message by removing the extras '#' at the end
     *
     * @param message String to unformat
     * @param size    int the exact length of the message
     * @return an unformatted message
     * @throws BadMessageFormatException
     */
    public static String unFormatMessage(String message, int size) throws BadMessageFormatException {
        int len = message.length();
        if (len != size)
            throw new BadMessageFormatException("Message length != given size");

        int k = len;
        while (message.charAt(k - 1) == '#')
            k--;
        return message.substring(0, k);
    }

    /**
     * Unformats a number by removing the extras '0' at the beginning
     *
     * @param strNumber String the formatted number
     * @return int the number unformatted
     * @throws BadMessageFormatException if the number is not valid
     */
    public static int unFormatNumber(String strNumber) throws BadNumberFormatException {
        try {
            return Integer.valueOf(strNumber);
        } catch (Exception e) {
            throw new BadNumberFormatException("Not a number : " + strNumber);
        }
    }

    /**
     * Unformats an IPv4 address, by compressing the extras '0' when necessary
     *
     * @param ip String formatted IPv4 address
     * @return an unformatted IPv4 address
     * @throws BadMessageFormatException if it is not in a formatted IPv4 address
     */
    public static String unFormatIPv4(String ip) throws BadIpFormatException {
        if (!ip.matches("[0-9]{3}\\.[0-9]{3}\\.[0-9]{3}\\.[0-9]{3}"))
            throw new BadIpFormatException("Bad IPv4 Format, ip:" + ip);

        String newIp = "";
        String[] ipParts = ip.split("\\.");
        int len = ipParts.length;
        int j;
        int len2 = 0; // Accepts IP parts with length {1,3} (unformatted)

        for (int i = 0; i < len; i++) {
            j = 0;
            len2 = ipParts[i].length();
            while (j < len2 && ipParts[i].charAt(j) == '0') // Find Index after unwanted zeros '0'
                j++;
            if (j == len2) // Case : "000" (full zeros)
                newIp += '0';
            else if (j == 0) // Case : No Need to UnFormat
                newIp += ipParts[i];
            else // Other Cases
                newIp += ipParts[i].substring(j, len2);
            if (i != len - 1)
                newIp += '.';
        }

        return newIp;
    }


    // Test purposes (Simple tests functions above)
    public static void main(String[] args) throws Exception {
        String str = "String Test 01";
        byte[] bytes = stringToByteArray(str);
        String str2 = byteArrayToString(bytes);

        // Convert Bytes to String, and String to Bytes Test
        System.out.println(str);
        System.out.println(str2);
        if (str.equals(str2))
            System.out.println("Convert Bytes and String OK");

        // Format Message Test
        System.out.println(formatMessage(str, 18)); // str(len=14) + "####"
        System.out.println(formatMessage(str, 8)); // "String T"

        // Format Number Test
        System.out.println(formatNumber(1, 3)); // 001
        System.out.println(formatNumber(5555, 4)); // 5555
        try {
            System.out.println(formatNumber(5555, 2)); // BadNumberFormatException
        } catch (Exception e) {}

        // Format IPv4 Test
        try {
            System.out.println(formatIPv4("127.0.0")); // BadIpFormatException
        } catch (Exception e) {}
        System.out.println(formatIPv4("127.0.0.1")); // 127.000.000.001
        System.out.println(formatIPv4("255.255.255.255")); // 255.255.255.255

        // UnFormat Message Test
        str2 = formatMessage(str, 20);
        System.out.println("With Format : " + str2);
        System.out.println("After unFormat : " + unFormatMessage(str2, 20)); // str
        try {
            System.out.println(unFormatMessage(str2, 18)); // BadMessageFormatException
        } catch (Exception e) {}

        // UnFormat IPv4 Test
        try {
            System.out.println(unFormatIPv4("127.0.0.1")); // BadIpFormatException
        } catch (Exception e) {}
        System.out.println(unFormatIPv4("000.000.000.000"));
        System.out.println(unFormatIPv4("001.102.030.000"));
        System.out.println(unFormatIPv4("255.255.255.255"));

        // UnFormat Number Test
        System.out.println(unFormatNumber("000")); // 0
        System.out.println(unFormatNumber("123")); // 123
        System.out.println(unFormatNumber("0123")); // 123
        System.out.println(unFormatNumber("00012030")); // 12030
        try {
            System.out.println(unFormatNumber("00az30")); // BadNumberFormatException
        } catch (Exception e) {}
    }
}