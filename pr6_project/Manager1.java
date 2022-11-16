import java.net.*;
import java.io.*;
import java.util.*;

public class Manager1 {

    private final int nbDifuseurMax;
    private final ServerSocket server;
    private final HashMap<Socket, String> difuseur;

    public Manager1(int port) throws Exception {
        if (port <= 1024 || port > 9999)
            throw new IllegalArgumentException("Invalid port");
        this.difuseur = new HashMap<Socket, String>();
        this.server = new ServerSocket(port);
        this.nbDifuseurMax = 99;
    }

    public void start() {
        try {
            while (true) {
                Socket socket = server.accept();
                Communication com = new Communication(socket);
                Thread t = new Thread(com);
                t.start();
            }
        } catch (Exception e) {
            System.out.println(e);
            e.printStackTrace();
        }
    }

    /**
     * Tries to REGIster a Diffuser to Manager
     * Sends back [REOK] or [RENO] for success or failure
     *
     * @param pw Diffuser's output stream
     * @param sock Diffuser's Socket
     * @param mess REGI message
     * @return boolean registered the diffuser
     * @throws Exception
     */
    public boolean REGI(PrintWriter pw, Socket sock, String mess) throws Exception {
        if (this.difuseur.size() < this.nbDifuseurMax && Parser.checkREGI(mess)) {
            this.difuseur.put(sock, mess.substring(5)); // without [REGI ]
            System.out.println(mess);
            pw.print("REOK\r\n");
            pw.flush();
            return true;
        }
        pw.print("RENO\r\n");
        pw.flush();
        sock.close();
        return false;
    }

    /**
     * LISTs the registered Diffusers
     *
     * @param pw User's output stream
     * @param s User's Socket
     * @throws Exception
     */
    public void LIST(PrintWriter pw, Socket s) throws Exception {
        pw.print("LINB " + Formatter.formatNumber(this.difuseur.size(), 2) + "\r\n");
        pw.flush();
        for (Map.Entry<Socket, String> mapentry : this.difuseur.entrySet()) {
            pw.print("ITEM" + " " + mapentry.getValue()); // value from REGI already has "\r\n"
            pw.flush();
        }
        s.close();
    }

    public void supprimer(Socket sock) throws IOException {
        this.difuseur.remove(sock);
        sock.close();
    }

    public static void main(String[] args) {
        if (args.length != 1) {
            System.out.println("Mettre un port en argument");
            return;
        }
        try {
            Manager1 g = new Manager1(Integer.valueOf(args[0]));
            g.start();
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    private class Communication implements Runnable {

        private Socket socket;
        private int delaiEnvoi;
        private int delaiAttente;

        public Communication(Socket s) {
            this.socket = s;
            this.delaiEnvoi = 20000;
            this.delaiAttente = 3000;
        }

        public boolean relance(BufferedReader br, PrintWriter pw) throws Exception {
            pw.print("RUOK\r\n");
            pw.flush();
            Thread.sleep(delaiAttente);
            String msg = Parser.readFullLine(br);
            if (!msg.equals("IMOK\r\n")) {
                supprimer(socket);
                return false;
            } else
                System.out.print(msg); // Received IMOK from Diffuser
            return true;
        }

        public void run() {
            try {
                BufferedReader br = new BufferedReader(new InputStreamReader(this.socket.getInputStream()));
                PrintWriter pw = new PrintWriter(new OutputStreamWriter(this.socket.getOutputStream()));
                String mess = Parser.readFullLine(br);

                if (mess.startsWith("REGI ")) {
                    if (!REGI(pw, this.socket, mess))
                        return;
                } else if (mess.equals("LIST\r\n")) {
                    System.out.println("Received [LIST]");
                    LIST(pw, this.socket);
                    return;
                } else {
                    pw.print("Message was not understood\n");
                    pw.flush();
                    return;
                }
                Thread.sleep(delaiEnvoi);
                while (relance(br, pw)) {
                    Thread.sleep(delaiEnvoi);
                }
            } catch (Exception e) {
                System.out.println(e.getMessage());
            }
        }
    }

}
