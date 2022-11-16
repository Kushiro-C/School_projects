import java.net.*;
import java.io.*;


public class Serv_Diff_rcv implements Runnable {
    private Socket socket;
    private Diffuser1 diffus;

    public Serv_Diff_rcv(Socket s, Diffuser1 diffus) {
        this.socket = s;
        this.diffus = diffus;
    }


    public void run() {
        try {
            BufferedReader br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            PrintWriter pw = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));
            String mess = Parser.readFullLine(br);
            if(mess.length() <= 0)
                return;
            /*while(mess == null ||  mess.length() <= 2 || mess.equals("\r\n") || !mess.endsWith("\n\n")) {
                mess = br.readLine();
                if(mess != null)
                    break;
            }*/
            System.out.println(mess);
            if((mess.substring(0, 4)).equals("LAST")) {
                if (!Parser.checkLAST(mess)) {
                    pw.print("Wrong Syntax with LAST\n");
                    pw.flush();
                    br.close();
                    pw.close();
                    socket.close();
                    return;
                }
                int num = Formatter.unFormatNumber(mess.substring(5, 8)); // num of the LAST messages, 3 bytes
                if (diffus.getLastMess().size() - num >= 0) {
                    diffus.iLastMess(num, pw);
                    pw.print("ENDM\r\n");
                    pw.flush();
                }
            } else if ((mess.substring(0, 4)).equals("MESS")) {
                if (!Parser.checkMESS(mess)) {
                    pw.print("Wrong Syntax with MESS\n");
                    pw.flush();
                    br.close();
                    pw.close();
                    socket.close();
                    return;
                }
                diffus.addMessages(mess.substring(5, 154));
                pw.print("ACKM\r\n");
                pw.flush();
            } else {
                System.out.print("Message recu : " + mess + "\r\n");
            }
            br.close();
            pw.close();
            socket.close();
        } catch (Exception e) {
            System.out.println(e);
            e.printStackTrace();
        }
    }
}
