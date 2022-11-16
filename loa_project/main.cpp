#include "Conteneur.hpp"
#include "Partie.hpp"
#include <sstream>

/**
 * Asks the user if he wants to start
 * a new game, or load from the saved file
 * @return int 1 = New Game, 2 = Load Game
 */
int startOrLoadGame() {
    cout << "======================================" << endl;
    cout << "Bienvenue dans notre jeu !" << endl << endl;
    cout << "(1) Lancer une nouvelle partie" << endl;
    cout << "(2) Charger votre partie" << endl << endl;
    cout << "Choisissez : ";

    int input;
    cin >> input;
    if (cin.fail()) { // In case the user doesn't type an int
        cin.clear();
        cin.ignore(INT32_MAX, '\n');
        input = -1;
    }
    cout << endl;
    cout << "======================================" << endl;

    return input;
}

/**
 * Asks user to choose his class
 * @return int The index of the class
 * 1 = Amazone, 2 = Guerrier, 3 = Moine, 4 = Sorcière
 */
int chooseClass() {
    cout << "======================================" << endl;
    cout << "Classes disponibles :" << endl;
    cout << "(1) Amazone" << endl;
    cout << "(2) Guerrier" << endl;
    cout << "(3) Moine" << endl;
    cout << "(4) Sorcière" << endl << endl;
    cout << "Choisissez votre classe : ";
    int input;
    cin >> input;
    if (cin.fail()) { // In case the user doesn't type an int
        cin.clear();
        cin.ignore(INT32_MAX, '\n');
        input = -1;
    }
    cout << endl;
    cout << "======================================" << endl;

    return input;
}

/**
 * @param pFile File stream
 * @return true File is empty
 * @return false File is not empty
 */
bool isFileEmpty(std::ifstream& pFile)
{
    return pFile.peek() == std::ifstream::traits_type::eof();
}

/**
 * @brief Loads a game from a file
 * We suppose the loaded file was not touched outside of this program
 * and it was saved with the method Partie::saveGameToFile()
 */
void loadGameFromFile()
{
    ifstream inFile ("gameData.txt"); // Open file

    if (inFile.fail()) // Error while opening inFile
    {
        cerr << "Failed to open the file." << endl;
        exit(1);
    }
    else // File successfully opened
    {
        // Stops the program if the file is empty
        if (isFileEmpty(inFile)) {
            inFile.close();
            cout << "Fichier de sauvegarde vide ou corrompu !" << endl;
            exit(1);
        }

        // Loads the data
        size_t pSize = 4;
        size_t castleSize = 5; // 5*5
        vector<pair<int, int>> m_pos;
        vector<int> m_nbDoors;
        vector<float> m_scores;
        vector<Personnage *> m_p;
        vector<double> currentHP;
        vector<vector<int>> m_bags; // Size of each bag is always 4
        vector <vector <vector<int> >> m_castleContents;

        // Loads m_pos
        for (size_t i = 0; i < pSize; i++) {
            int x;
            int y;
            inFile >> x >> y;
            m_pos.push_back(make_pair(x, y));
        }

        // Loads m_nbDoors
        for (size_t i = 0; i < pSize; i++) {
            int x;
            inFile >> x;
            m_nbDoors.push_back(x);
        }

        // Loads m_scores
        for (size_t i = 0; i < pSize; i++) {
            float x;
            inFile >> x;
            m_scores.push_back(x);
        }

        // Loads m_p
        for (size_t i = 0; i < pSize; i++) {
            string name; // Name of Personnage
            string className; // Class Name
            double hp; // Current HP
            vector<int> bag; // m_bag

            inFile >> name >> className >> hp;
            currentHP.push_back(hp);

            for (size_t j = 0; j < pSize; j++) { // m_bag
                int itemIndex;
                inFile >> itemIndex;
                bag.push_back(itemIndex);
            }

            Personnage *p = Personnage::factory(name, className);
            m_p.push_back(p);
            m_bags.push_back(bag);
        }

        // Load m_castle
        for (size_t i = 0; i < castleSize; i++) {
            vector<vector<int>> row;

            for (size_t j = 0; j < castleSize; j++) {
                vector<int> room;
                string line;
                getline(inFile, line, '\n'); // Get line (room contents)

                // Skip blank lines
                while (line.find_first_not_of(' ') == string::npos)
                    getline(inFile, line, '\n');

                // transform line into stringstream
                stringstream roomContents;
                roomContents.str(line);
                int itemIdx;

                // Get all item indexes in a line
                while (roomContents >> itemIdx)
                    room.push_back(itemIdx);

                row.push_back(room);
            }
            m_castleContents.push_back(row);
        }

        inFile.close();

        // Check size of vectors is correct
        if (m_p.size() != 4 || m_pos.size() != 4 || currentHP.size() != 4
            || m_nbDoors.size() != 4 || m_scores.size() != 4) {
            cout << "Erreur lors du chargement de la partie !" << endl;
            return;
        }

        // Load all data into Partie
        Partie partie {
            m_p, currentHP, m_pos, m_nbDoors, m_scores,
            m_bags, m_castleContents
        };
        cout << "Partie chargée." << endl;
        partie.game_start(); // Resume game
    }

}

// launcher
int main()
{
    int inputNewGame = startOrLoadGame();
    while (inputNewGame <= 0 || inputNewGame > 2) // Must be 1 or 2
        inputNewGame = startOrLoadGame();

    if (inputNewGame == 1) // New game
    {
        int input = chooseClass();
        while (input <= 0 || input > 4) // Wrong class indexes (1-4)
            input = chooseClass();

        // Personnages
        Personnage *persA = Personnage::factory("NPC_Amazone", "amazone");
        Personnage *persG = Personnage::factory("NPC_Guerrier", "guerrier");
        Personnage *persM = Personnage::factory("NPC_Moine", "moine");
        Personnage *persS = Personnage::factory("NPC_Sorciere", "sorciere");
        Personnage *userPers;

        // Start the game with your chosen class
        if (input == 1) { // User is Amazone
            userPers = Personnage::factory("Joueur_Amazone", "amazone");
            Partie partie {*userPers, *persG, *persM, *persS};
            partie.game_start();
        } else if (input == 2) { // User is Guerrier
            userPers = Personnage::factory("Joueur_Guerrier", "guerrier");
            Partie partie {*userPers, *persS, *persM, *persA};
            partie.game_start();
        } else if (input == 3) { // User is Moine
            userPers = Personnage::factory("Joueur_Moine", "moine");
            Partie partie {*userPers, *persG, *persS, *persA};
            partie.game_start();
        } else if (input == 4) { // User is Sorcière
            userPers = Personnage::factory("Joueur_Sorciere", "sorciere");
            Partie partie {*userPers, *persG, *persM, *persA};
            partie.game_start();
        }
    }

    else if (inputNewGame == 2) // Load Game
    {
        loadGameFromFile(); // Load the game
    }

    else
        cout << "Une erreur est survenue dans le main." << endl;

    return 0;
}
