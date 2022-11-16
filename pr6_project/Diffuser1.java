import java.net.*;
import java.util.ArrayList;
import java.io.*;
import java.util.Scanner;

public class Diffuser1 {
    private final String id; // Formatted
    private final int multi_port;
    private final int recv_port;
    private final String adress; // Formatted
    private final ArrayList<String> messages = new ArrayList<>(); // 0 to 9999
    private final ArrayList<String> lastMess = new ArrayList<>(); // 0 to 999
    private int current = 0; // Message index to send

    public Diffuser1(String id, int multi_port, int recv_port, String adress) throws BadIpFormatException {
        if(multi_port > 9999 || recv_port > 9999) {
            System.out.println("il faut des ports inférieurs à 9999");
            System.exit(0);
        }
        this.id = Formatter.formatMessage(id, 8);
        this.multi_port = multi_port;
        this.recv_port = recv_port;
        this.adress = Formatter.formatIPv4(adress);
    }

    /**
     * read a file and fill this.messages
     *
     * @param fileName
     */
    private void fillDefaultMessages(String fileName) {
        try {
            File myObj = new File(fileName);
            Scanner myReader = new Scanner(myObj);
            while (myReader.hasNextLine()) {
                String data = myReader.nextLine();
                if (data.length() > 140)
                    continue; // Or throws ?
                messages.add(id + " " + Formatter.formatMessage(data, 140));
                //System.out.println("ajout: " + data);
            }
            myReader.close();
        } catch (FileNotFoundException e) {
            System.out.println("An error occurred.");
            e.printStackTrace();
        }
    }

    /**
     * add mess in messages (list of messages stocked in diffuser)
     *
     * @param mess
     */
    protected synchronized void addMessages(String mess) { // change it to private ?
        messages.add(mess);
        if (messages.size() > 10000)
            messages.remove(0);
        //System.out.println("ajout de " +messages.get(messages.size()-1));
    }

    /**
     * get last i messages and print on PrintWriter call this function in class
     * Serv_Diff_rcv
     *
     * @param i  last messages
     * @param pw PrintWriter TCP
     */
    public void iLastMess(int i, PrintWriter pw) {
        synchronized (lastMess) {
            for (int j = lastMess.size() - i; j < lastMess.size(); j++) {
                pw.print(lastMess.get(j));
                pw.flush();
            }
        }
    }

    /**
     * Gets the next message to send, and increments current counter
     *
     * @return String the next message to send
     */
    private synchronized String getMessagesIncr() {
        String s = messages.get(current); // Already Formatted when added
        current = (current + 1) % messages.size();
        return s;
    }

    /**
     * add mess in messages (list of messages stocked in diffuser)
     *
     * @param mess
     */
    protected synchronized void addLastMess(String mess) { // change it to private ?
        lastMess.add(mess);
        if (lastMess.size() > 1000)
            lastMess.remove(0);
    }

    /**
     * class used for multicast
     */
    private class Diffu_communic implements Runnable {
        public Diffuser1 diffus;
        private int cpt; //diffuser's current or cpt = 0 ?

        public Diffu_communic(Diffuser1 diffus) {
            this.diffus = diffus;
            cpt = diffus.current; // nedd sync ? 0 at the beginning
        }

        public void run() {
            try {
                DatagramSocket dso = new DatagramSocket();
                InetSocketAddress ia = new InetSocketAddress(Formatter.unFormatIPv4(diffus.adress), diffus.multi_port);
                DatagramPacket paquet;
                byte[] data = new byte[100];
                String mess;
                String diff;// DIFF message to send
                String old; // OLDM message to send

                while (!dso.isClosed()) {
                    mess = getMessagesIncr(); // Already Formatted
                    diff = "DIFF " + Formatter.formatNumber(cpt, 4) + " " + mess + "\r\n";
                    data = diff.getBytes();
                    paquet = new DatagramPacket(data, data.length, ia);
                    dso.send(paquet);
                    old = "OLDM " + Formatter.formatNumber(cpt, 4) + " " + mess + "\r\n";
                    addLastMess(old);
                    cpt = (++cpt) % 10000;
                    Thread.sleep(5000); // Frequency send messages
                }
                dso.close();

            } catch (Exception e) {
                System.out.println(e);
                e.printStackTrace();
            }
        }
    }

    private class Diffu_Manager implements Runnable {

        private final Socket socket;
        private final String adresse_Diffuser;

        public void run() {
            try {
                String mess = "REGI " + id + " " + adress + " " + multi_port + " " + adresse_Diffuser + " " + recv_port;
                BufferedReader br = new BufferedReader(new InputStreamReader(this.socket.getInputStream()));
                PrintWriter pw = new PrintWriter(new OutputStreamWriter(this.socket.getOutputStream()));
                pw.print(mess + "\r\n");
                pw.flush();
                mess = Parser.readFullLine(br);
                System.out.println(mess);
                if (!mess.equals("REOK\r\n")) {
                    System.out.println("Echec REGI");
                    return;
                }
                while (true) {
                    mess = Parser.readFullLine(br);
                    System.out.println(mess);
                    if (mess.equals("RUOK\r\n")) {
                        pw.print("IMOK\r\n");
                        pw.flush();
                    } else {
                        System.out.println("No manager !\r\n");
                        System.exit(0);
                    }
                }
            } catch (Exception e) {
                System.out.println(e.getMessage());
                return;
            }
        }

        public Diffu_Manager(String addresse_Manager, String adresse_Diffu, int port) throws Exception {
            this.socket = new Socket(Formatter.formatIPv4(addresse_Manager), port);
            this.adresse_Diffuser = Formatter.formatIPv4(adresse_Diffu);
        }

    }

    /**
     * class used for TCP receive
     */
    private class Diffu_rcv implements Runnable {
        public Diffuser1 diffus;

        public Diffu_rcv(Diffuser1 diffus) {
            this.diffus = diffus;
        }

        public void run() {
            try {
                ServerSocket server = new ServerSocket(diffus.recv_port);
                while (!server.isClosed()) {
                    // recv
                    Socket socket = server.accept();
                    Serv_Diff_rcv service = new Serv_Diff_rcv(socket, diffus);
                    Thread t = new Thread(service);
                    t.start();
                }
                server.close();
            } catch (Exception e) {
                System.out.println(e);
                e.printStackTrace();
            }
        }
    }

    /**
     * toString diffuser
     */
    public String toString() {
        String s;
        s = "-----------DIFFUSER---------------\n";
        s = s + "id = " + id + "\n";
        s = s + "adresse = " + adress + "\n";
        s = s + "port de multicast = " + multi_port + "\n";
        s = s + "port de reception TCP = " + recv_port + "\n";
        s = s + "----------------------------------\n";
        return s;
    }

    public static String[] getArg(String pathFile){
        String param = "";
        try{
            File file = new File(pathFile);
            FileReader fr = new FileReader(file);
            BufferedReader br = new BufferedReader(fr);
            String line;
            int l = 1;
            param = br.readLine();
            while((line = br.readLine()) != null && l < 4){
                param = param + ";" + line;
                l++;}
            fr.close();}
        catch(IOException e){
            e.printStackTrace();}
        return param.split(";");
    }

    public static void main(String[] args) {
        if (args.length != 1) {
            System.out.println("Fichier de configuration requis en parametre");
            return;
        }
        String[] arg = getArg(args[0]);
        if (arg.length != 4) {
            System.out.println("Usage : Param1 = MultiPort, Param2 = RecvPort, Param3 = ManagerAdresse ,Param4 = ManagerPort");
            return;
        }
        if (Integer.valueOf(arg[0]) > 9999 || Integer.valueOf(arg[1]) > 9999) {
            System.out.println("please need port < 9999");
            return;
        }
        if (Integer.valueOf(arg[0]) < 1024 || Integer.valueOf(arg[1]) < 1024) {
            System.out.println("please do not use port < 1024");
            return;
        }
        try {
            Diffuser1 diffus = new Diffuser1("diffuID", Integer.valueOf(arg[0]), Integer.valueOf(arg[1]), "225.1.2.4");
            diffus.fillDefaultMessages("diff_mess.txt");
            System.out.println(diffus);

            // register manager
            // The first argument is the adress of the Manager
            // The second is the current adresse ipv4
            // The third is the port of the manager
            Diffu_Manager dif_man = diffus.new Diffu_Manager(arg[2], "127.0.0.1", Integer.valueOf(arg[3]));
            Thread t3 = new Thread(dif_man);
            t3.start();

            // multicast
            Diffu_communic multi_diffus = diffus.new Diffu_communic(diffus);
            Thread t1 = new Thread(multi_diffus);
            t1.start();

            // recv
            Diffu_rcv recv_diffus = diffus.new Diffu_rcv(diffus);
            Thread t2 = new Thread(recv_diffus);
            t2.start();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public ArrayList<String> getMessages() {
        return messages;
    }

    public ArrayList<String> getLastMess() {
        return lastMess;
    }

}
