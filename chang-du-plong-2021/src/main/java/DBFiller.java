
import java.io.*;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

public class DBFiller {

    public static final File musicFolder = new File("res/music");
    private static Connection connection = null;
    private static Statement stmt = null;
    public static final String dbInfo = "jdbc:postgresql://localhost:5432/plong_chang_du"; // vincent -> DATABASE NAME
    public static final String username = "plong_chang_du"; // user id
    public static final String password = "1234"; // password
    static int BUF_SIZE = 44100;
    static int cptTest = 0; // number of fingerPrintGenerator function call

    /**
     * read musics folder and fill databse
     * @throws IOException
     * @return 1 if already filled, else 0
     */
    public static int fillDataBase() throws IOException {
        // check if DB is already filled
        // if the file res/checker contains 1
        // it means database filled. else it contains 0
        String oldFileName = "res/checker";
        String tmpFileName = "res/tmp_.dat";

        BufferedReader br = null;
        BufferedWriter bw = null;
        try {
            br = new BufferedReader(new FileReader(oldFileName));
            bw = new BufferedWriter(new FileWriter(tmpFileName));
            String line;
            if ((line = br.readLine()) != null) {
                if (line.contains("0")) //if not filled
                    bw.write("1");
                else {   //else exit
                    System.out.println("----Database already or partially filled.----\n");
                    File newFile = new File(tmpFileName);
                    newFile.delete();
                    br.close();
                    bw.close();
                    return 1;
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        br.close();
        bw.close();
        // delete old file..
        File oldFile = new File(oldFileName);
        oldFile.delete();

        // replace old file by the new file with correct value
        File newFile = new File(tmpFileName);
        newFile.renameTo(oldFile);

        // init connection with DataBase
        try {
            /*
             * Runtime.getRuntime().exec( "psql -f src/main/java/DataBase.sql" ); //with
             * good username role in databse (SET ROLE plong;)
             * Thread.sleep(2000);
             */

            // -- important for tests----
            // exec in psql before executing project:
            // SET ROLE plong;
            // \i src/main/java/DataBase.sql
            // -----

            // run DataBase.sql file
            Runtime.getRuntime().exec( "psql -f res/sql/DataBase.sql" );

            Class.forName("org.postgresql.Driver");
            connection = DriverManager
                    .getConnection(dbInfo,
                            username, password);

            stmt = connection.createStatement();
            System.out.println("Opened database successfully.");

            // stmt.executeUpdate("SET ROLE plong;");
            // stmt.executeUpdate("\\i src/main/java/DataBase.sql");

        } catch (Exception e) {
            e.printStackTrace();
            System.err.println(e.getClass().getName() + ": " + e.getMessage());
            System.exit(0);
        }

        // read each music in Musics folder
        for (File childFile : musicFolder.listFiles()) {
            if (childFile.isDirectory()) {
                readGenreFolder(childFile);
            } else {
                // System.out.println(childFile.getName());
                continue;
            }
        }

        // Succes display
        System.out.println("------Database successfully filled-------\n");
        System.out.println("-------Please launch the app again-------\n");
        try {
            connection.close();
            stmt.close();
        } catch (SQLException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return 0;
    }

    /**
     * read folder(its name is the genre of the musics that
     * it contain)
     * and fill database with appropriate genre for each music
     *
     * @param folder
     */
    public static void readGenreFolder(File folder) {
        for (File childFile : folder.listFiles()) {
            if (childFile.isDirectory()) {
                continue;
            } else {
                //System.out.println(folder.getName() + " :" + childFile.getName());

                // generate finger print
                /*
                 * byte[] bytes;
                 * double fingerPrintArray[];
                 * try {
                 * bytes = Files.readAllBytes(childFile.toPath());
                 * fingerPrintArray = Calcul.fingerPrintGenerator(bytes);
                 * } catch (IOException e) {
                 * System.out.println("error childFile error");
                 * e.printStackTrace();
                 * }
                 */

                // insert into musics appropriate values
                String sqlMusic = "insert into musics (name) values ('" + childFile.getName() + "');";

                // get id of the last inserted music
                String sqlid = "SELECT MAX(music_id) FROM musics;";

                ResultSet res;
                int id = 0; // buffer for last inserted music id

                try {
                    stmt.executeUpdate(sqlMusic);
                    res = stmt.executeQuery(sqlid);
                    if (res.next()) {
                        id = res.getInt("max");
                        res.close();
                    }

                    // insert classification
                    String sql = "insert into classification values ('" + String.valueOf(id)
                            + "','" + folder.getName()
                            + "');";
                    stmt.executeUpdate(sql);

                    // insert fingerprints
                    insertFingerPrint(childFile.getPath(), id);
                } catch (SQLException e) {
                    e.printStackTrace();
                } catch (IOException e) {
                    e.printStackTrace();
                }

            }
        }
    }

    /**
     * read filePath and create its firsts fingerprints
     * and insert it in database
     *
     * @param filePath path of the file
     * @throws IOException
     * @throws SQLException
     */
    public static void insertFingerPrint(String filePath, int musicId) throws IOException, SQLException {
        InputStream inputstream;
        inputstream = new FileInputStream(filePath);
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        byte[] buffer = new byte[BUF_SIZE];
        int bytesRead;

        System.out.println("inserting " + filePath + " into musics_fingerPr table ..");

        // read file and generate fingerprint
        while ((bytesRead = inputstream.read(buffer)) > 0) {
            baos.write(buffer, 0, bytesRead);
            byte audio[] = baos.toByteArray();
            double[] res = Calcul.fingerPrintGenerator2(audio);
            cptTest++;

            // inserting values
            for (int i = 0; i < res.length; i++) {
                // insert classification
                String sql = "insert into musics_fingerPr values ('" + musicId + "','" + res[i] + "');";
                stmt.executeUpdate(sql);
            }
        }

        inputstream.close();
        System.out.println("cptTest after " + filePath + " :" + cptTest);
    }


    /**
     * connect database and return its Statement
     * @return the Statement
     */
    public static Statement initConnection() {
        Statement statement = null;
        try {
            Class.forName("org.postgresql.Driver");
            Connection connec = DriverManager
                .getConnection(dbInfo,
                        username, password);

        statement = connec.createStatement();
        } catch (ClassNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        catch (SQLException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        return statement;
    }
}
