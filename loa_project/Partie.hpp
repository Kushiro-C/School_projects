#ifndef PARTIE_HPP
#define PARTIE_HPP

#include <fstream>
#include "Personnage.hpp"

using namespace std;

class Partie {

    private :
        const vector<Personnage *> m_p;
        vector<pair<int, int>> m_pos; // position of players
        // vect<bool> has always 4 bools (Doors) for North East South West
        vector <vector // Grid of Room (pair <vect Conteneur, vect Doors>)
            <pair <vector<Conteneur*>, const vector<bool>> >> m_castle;
        vector<int> m_nbDoors; // number door opened
        vector<float> m_scores; // 1 score per win in a duel

        void initMap(); // init map called in constructor
        void addPlayersAndLoots(); // Randomly adds players and some Objet
        void displayPossibleActions() const;
        // Execute an action as a player
        void doUseItem(Personnage &p, Objet *obj, size_t idx);
        void doPickItem(Personnage &p, Objet *obj, size_t i, size_t j);
        void doPutItem(Personnage &p, Objet *obj, size_t i, size_t j);
        bool doMove(Personnage &p, size_t i, size_t j, size_t direction, size_t idx);
        // Asks user to do choose a specific action
        void askUserUseItem();
        void askUserPickItem();
        void askUserPutItem();
        bool askUserMove();
        bool askUserAction(); // Asks the user to choose one of the actions

        void duel(Personnage &p1, Personnage &p2); // battle between 2 Personnage
        void autoBattle(); // Automatic battles when 2 or more people are in the same room
        void userTurn(); // For the user only (will ask to input actions)
        void doRandomActions(Personnage &p, size_t i); // for NPCs
        void removeContentFromRoom(size_t i, size_t j, Conteneur *c);

        void saveGameToFile(); // Saves the current game into the file

    public :
        Partie(Personnage &j1,Personnage &p2, Personnage &p3, Personnage &j4);
        Partie(vector<Personnage *> p, vector<double> hp, vector<pair<int, int>> pos,
                vector<int> nbDoors, vector <float> scores, vector<vector<int>> bags,
                vector<vector<vector<int>>> castle);
        friend ostream& operator<<(ostream& out, Partie& x); // operator display

        bool checkFinish() const; // check if only one player alive
        void game_start(); // start the game

        void displayCastle() const; // for test
        void displayPlayers() const;
        void miniMap() const;

        bool isPlayerAlive() const;
        pair<int, int> getPlayerPosition(size_t i) const;
        int getNbDoorP1() const;
        int getNbDoorP2() const;
        int getNbDoorP3() const;
        int getNbDoorP4() const;
        float getScoreP1() const;
        float getScoreP2() const;
        float getScoreP3() const;
        float getScoreP4() const;
        vector<int> getNbDoors() const;
        vector<float> getScores() const;
        vector<string> getNames() const;
        bool getNorthDoor(size_t i, size_t j) const;
        bool getEastDoor(size_t i, size_t j) const;
        bool getSouthDoor(size_t i, size_t j) const;
        bool getWestDoor(size_t i, size_t j) const;
        ~Partie(); // destructor
};
template<typename Base, typename T> bool instanceof(const T *ptr); //instance of


#endif