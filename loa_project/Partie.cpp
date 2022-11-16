#include "Partie.hpp"
#include "Cle.hpp"
#include "Bouteille.hpp"

/**
 * Partie constructor with exactly 4 Personnages
**/
Partie::Partie(Personnage &j1,Personnage &p2, Personnage &p3, Personnage &p4)
    : m_p({&j1, &p2, &p3, &p4})
{
    for (size_t i = 0; i < 4; i++) {
        m_nbDoors.push_back(0);
        m_scores.push_back(0);
        m_pos.push_back(make_pair(-1, -1)); // reset pos in addPlayersAndLoots()
    }
    initMap();
    addPlayersAndLoots();
}

/**
 * Constructor when loading a game
 * It restores the saved data
 */
Partie::Partie(vector<Personnage *> p, vector<double> hp, vector<pair<int, int>> pos,
                vector<int> nbDoors, vector <float> scores, vector<vector<int>> bags,
                vector<vector<vector<int>>> castle)

                : m_p(p), m_pos(pos), m_nbDoors(nbDoors), m_scores(scores)
{
    // Init basic castle layout
    initMap();

    // Add the players back
    for (size_t i = 0; i < m_pos.size(); i++) {
        int x = m_pos[i].first;
        int y = m_pos[i].second;
        m_castle[x][y].first.push_back(m_p[i]);
    }

    // Add the items back
    for (size_t i = 0; i < castle.size(); i++) {
        for (size_t j = 0; j < castle[i].size(); j++) {
            for (auto idx : castle[i][j]) {
                if (idx != -1) {
                    Objet *o = Objet::getItemAt(idx);
                    if (o != nullptr)
                        m_castle[i][j].first.push_back(o);
                }
            }
        }
    }

    // Clear player bags
    for (size_t i = 0; i < m_p.size(); i++) {
        for (auto item : m_p[i]->getBag())
            m_p[i]->removeItemFromBag(item);

        // Add the items from bag back to the player
        for (size_t j = 0; j < bags[i].size(); j++) {
            if (bags[i][j] != -1) { // Item index
                Objet *o = Objet::getItemAt(bags[i][j]);
                if (o != nullptr)
                    m_p[i]->addItemToBag(o);
            }
        }
    }

    // Update current Players HPs
    for (size_t i = 0; i < hp.size(); i++)
        m_p[i]->setHealth(hp[i]);
}

/**
 * display partie, players, scores, status of the game
**/
ostream& operator<<(ostream& out, Partie& x)
{
    out << "==============================" << endl;
    if (x.checkFinish()) { // Game ended
        out << "Partie terminée !" << endl;
        if (x.isPlayerAlive())  // User won
            out << "Bravo vous avez gagné !" << endl;
        else                    // User lost
            out << "Vous avez perdu !" << endl;
    }
    vector<string> names = x.getNames();
    vector<float> scores = x.getScores();
    vector<int> nbDoors = x.getNbDoors();
    out << "Scores : " << endl;
    for (size_t i = 0; i < names.size(); i++) {
        out << names[i] << " a comme score : " << scores[i]
            << " et a ouvert " << nbDoors[i] << " portes !" << endl;
    }

    out << "==============================" << endl;

    return out;
}

/**
 * check if the game is finished, if all players except one are dead
 * or if player is dead (NPCs alive)
**/
bool Partie::checkFinish() const
{
    if (m_p.size() > 0 && m_p[0]->isDead()) // Player dead
        return true;
    size_t cpt = 0; // Count number of Player/NPCs dead
    for (size_t i = 0; i < m_p.size(); i++)
        if (m_p[i]->isDead())
            cpt++;
    return (cpt >= (m_p.size() -1));
}

/**
 * Displays all possible actions
 * Actions that are not available are still shown
 */
void Partie::displayPossibleActions() const
{
    cout << "##############################" << endl;
    cout << "(1) Utiliser un objet" << endl;
    cout << "(2) Ramasser un objet" << endl;
    cout << "(3) Poser un objet" << endl;
    cout << "(4) Se déplacer" << endl;
    cout << "(5) Afficher la mini carte" << endl;
    cout << "(6) Afficher mon statut" << endl;
    cout << "(7) Sauvegarder la partie" << endl;
    cout << "##############################" << endl << endl;
}

/**
 * Uses an Item
 * @param p Personnage doing the action
 * @param obj Item to Use
 * @param idx Index of player in vectors
 */
void Partie::doUseItem(Personnage &p, Objet *obj, size_t idx)
{
    if (obj != nullptr && p.hasItemInBag(obj))
    {
        int i = m_pos[idx].first; // Player coordinates
        int j = m_pos[idx].second;

        if (obj->getType() == 'k') { // Key
            Cle *key = dynamic_cast<Cle *>(obj);
            int x = key->getX(); // Key coordinates
            int y = key->getY();

            // Random Coordinates Teleport
            if (x == -1 || y == -1) {
                int castleSize = m_castle.size() - 1;
                x = rand()%(castleSize + 1);
                y = rand()%(castleSize + 1);
            }
            // Teleports the player at key's coordinates
            removeContentFromRoom(i, j, &p); // Remove player from current room
            m_castle[x][y].first.push_back(&p); // Add back player after teleport
            m_pos[idx].first = x;
            m_pos[idx].second = y;
            p.removeItemFromBag(obj); // Remove key from your bag
            cout << "Téléportation !!!" << endl;

        } else if (obj->getType() == 'b') { // Potion
            Bouteille *potion = dynamic_cast<Bouteille *>(obj);
            if (potion->getBouteille_type() == 0) // Poison
                cout << "Mieux vaut ne pas utiliser cette potion sur moi..." << endl;
            else { // 1 = Healing
                potion->use(p); // Use healing potion on yourself
                p.removeItemFromBag(obj); // Remove potion from your bag
                cout << "Potion utilisée !" << endl;
            }
        } else {
            cout << "Ce n'est pas un objet utilisable !" << endl;
            return;
        }
    } else
        cout << "L'objet n'a pas pu être utilisé." << endl;
}

/**
 * Picks an item from the room and put it in p's bag
 * @param p Personnage doing the action
 * @param obj Item to pick from the room
 * @param i Coordinate x of the room he is in
 * @param j Coordinate y of the room he is in
 */
void Partie::doPickItem(Personnage &p, Objet *obj, size_t i, size_t j)
{
    if (obj != nullptr && i >= 0 && j >= 0 && i < m_castle.size() && j < m_castle[i].size())
    {
        // Check if the bag is full
        if (p.isBagFull()) {
            cout << "Impossible de ramasser l'objet, le sac est rempli !" << endl;
            return;
        }

        // Check if the item obj is in the room (i, j)
        bool found = false;
        for (size_t k = 0; k < m_castle[i][j].first.size(); k++) {
            if (m_castle[i][j].first[k] == obj) // Item found
                found = true;
        }
        cout << "Objet ramassé." << endl;

        if (found) { // Item is in the room
            p.addItemToBag(obj); // Add item in player's bag
            removeContentFromRoom(i, j, obj); // Remove item from room
        }
    } else
        cout << "L'objet n'a pas pu être ramassé." << endl;
}

/**
 * Puts an item from p's bag into the room
 * @param p Personnage doing the action
 * @param obj Item to put in the room
 * @param i Coordinate x of the room he is in
 * @param j Coordinate y of the room he is in
 */
void Partie::doPutItem(Personnage &p, Objet *obj, size_t i, size_t j)
{
    if (obj != nullptr && i >= 0 && j >= 0 && i < m_castle.size() && j < m_castle[i].size())
    {
        // Check if the player has the item obj
        if (p.hasItemInBag(obj)) {
            p.removeItemFromBag(obj); // Remove item from player's bag
            m_castle[i][j].first.push_back(obj); // Add item in room
            cout << "Objet déposé." << endl;
        }
    } else
        cout << "L'objet n'a pas pu être déposé." << endl;
}

/**
 * Moves a Personnage through up to one Door
 * @param p Personnage doing the action
 * @param i Coordinate x of the room he is in
 * @param j Coordinate y of the room he is in
 * @param direction [1-5] for [North, East, South, West, NoMove]
 * @param idx index of the player in vector (0 for the user)
 */
bool Partie::doMove(Personnage &p, size_t i, size_t j, size_t direction, size_t idx)
{
    if (i < 0 || j < 0 || i > m_castle.size() || j > m_castle[i].size()
        || idx < 0 || idx >= m_p.size()) {
        cout << "Erreur position du joueur" << endl;
        return false;
    }

    if (direction == 6) { // No Move
        cout << p.getName() << " ne s'est pas déplacé." << endl;
        return true;
    }

    if (direction == 5) { // North
        if (getNorthDoor(i, j)) { // Door is open
            removeContentFromRoom(i, j, &p); // Remove player from current room
            i--;
            m_pos[idx].first = i; // Pos go up
            cout << p.getName() << " s'est déplacé au Nord." << endl;
        } else {
            cout << "Mouvement non autorisé" << endl;
            return false;
        }
    } else if (direction == 3) { // East
        if (getEastDoor(i, j)) { // Door is open
            removeContentFromRoom(i, j, &p); // Remove player from current room
            j++;
            m_pos[idx].second = j; // Pos go right
            cout << p.getName() << " s'est déplacé à l'Est." << endl;
        } else {
            cout << "Mouvement non autorisé" << endl;
            return false;
        }
    } else if (direction == 2) { // South
        if (getSouthDoor(i, j)) { // Door is open
            removeContentFromRoom(i, j, &p); // Remove player from current room
            i++;
            m_pos[idx].first = i; // Pos go down
            cout << p.getName() << " s'est déplacé au Sud." << endl;
        } else {
            cout << "Mouvement non autorisé" << endl;
            return false;
        }
    } else if (direction == 1) { // West
        if (getWestDoor(i, j)) { // Door is open
            removeContentFromRoom(i, j, &p); // Remove player from current room
            j--;
            m_pos[idx].second = j; // Pos go left
            cout << p.getName() << " s'est déplacé à l'Ouest." << endl;
        } else {
            cout << "Mouvement non autorisé" << endl;
            return false;
        }
    } else { // direction not recognized
        cout << "Mauvaise direction" << endl;
        return false;
    }

    m_castle[i][j].first.push_back(&p); // Add back player after the move
    m_nbDoors[idx]++;

    return true;
}

/**
 * Asks user to use an item
 * It will not wait if there are not usable items
 * Usable items are Bouteille and Cle
 */
void Partie::askUserUseItem()
{
    vector<Objet *> bag = m_p[0]->getBag();
    Objet *obj;
    int objCount = 0;

    cout << "******************************" << endl;
    for (size_t idx = 0; idx < bag.size(); idx++) {
        if (bag[idx]->getType() == 'b' || bag[idx]->getType() == 'k') {
            cout << "(" << idx + 1 << ") ";
            cout << *bag[idx] << endl;
            objCount++;
        }
    }
    if (objCount == 0)
        cout << "Inventaire vide" << endl;
    cout << "******************************" << endl << endl;

    if (objCount > 0) {
        int input;
        cout << "Choisissez l'objet à utiliser [1-4] : ";
        cin >> input;
        if (cin.fail()) { // In case the user doesn't type an int
            cin.clear();
            cin.ignore(INT32_MAX, '\n');
        }
        cout << endl;

        input--;
        if (input >= 0 && input < (int)bag.size()) {
            obj = bag[input];
            doUseItem(*m_p[0], obj, 0);
        }
    }
}

/**
 * Asks user to pick an item from the room
 * It will not wait if there are not pickable items
 */
void Partie::askUserPickItem()
{
    size_t i = m_pos[0].first; // User's coordinates
    size_t j = m_pos[0].second;
    Objet *obj;
    int objCount = 0;

    cout << "******************************" << endl;
    for (size_t idx = 0; idx < m_castle[i][j].first.size(); idx++) {
        if (!instanceof<Personnage>(m_castle[i][j].first[idx])) {
            cout << "(" << idx + 1 << ") ";
            cout << *m_castle[i][j].first[idx] << endl;
            objCount++;
        }
    }
    if (objCount <= 0)
        cout << "Aucun Objet à rammasser dans la pièce" << endl;
    cout << "******************************" << endl << endl;

    if (objCount > 0) {
        int input;
        cout << "Choisissez l'objet à ramasser [1-n] : ";
        cin >> input;
        if (cin.fail()) { // In case the user doesn't type an int
            cin.clear();
            cin.ignore(INT32_MAX, '\n');
        }
        cout << endl;

        input--;
        if (input >= 0 && input < (int)m_castle[i][j].first.size()
            && !instanceof<Personnage>(m_castle[i][j].first[input])) {
            obj = dynamic_cast<Objet *>(m_castle[i][j].first[input]);
            doPickItem(*m_p[0], obj, i, j);
        } else
            cout << "Action non valide." << endl;
    }
}

/**
 * Asks user to put an item in the room
 * It will not wait if there are not items in the bag
 */
void Partie::askUserPutItem()
{
    size_t i = m_pos[0].first; // User's coordinates
    size_t j = m_pos[0].second;
    vector<Objet *> bag = m_p[0]->getBag();
    Objet *obj;

    cout << "******************************" << endl;
    if (bag.size() == 0)
        cout << "Inventaire vide" << endl;
    for (size_t idx = 0; idx < bag.size(); idx++) {
        cout << "(" << idx + 1 << ") ";
        cout << *bag[idx] << endl;
    }
    cout << "******************************" << endl << endl;

    if (bag.size() > 0) {
        int input;
        cout << "Choisissez l'objet à déposer [1-4] : ";
        cin >> input;
        if (cin.fail()) { // In case the user doesn't type an int
            cin.clear();
            cin.ignore(INT32_MAX, '\n');
        }
        cout << endl;

        input--;
        if (input >= 0 && input < (int)bag.size()) {
            obj = bag[input];
            doPutItem(*m_p[0], obj, i, j);
        } else
            cout << "Action non valide." << endl;
    }
}

/**
 * Asks user to Move
 * You can only move in one of the available directions
 * But you can also choose not to move while ending your turn
 */
bool Partie::askUserMove()
{
    size_t i = m_pos[0].first; // User's coordinates
    size_t j = m_pos[0].second;

    cout << "******************************" << endl;
    if (getEastDoor(i, j)) {
        cout << "(1) Prendre la porte Est" << endl;
    }
    if (getSouthDoor(i, j)) {
        cout << "(2) Prendre la porte Sud" << endl;
    }
    if (getWestDoor(i, j)) {
        cout << "(3) Prendre la porte Ouest" << endl;
    }
    if (getNorthDoor(i, j)) {
        cout << "(5) Prendre la porte Nord" << endl;
    }
    cout << "(6) Ne pas se déplacer (termine le tour)" << endl;
    cout << "******************************" << endl << endl;

    int input;
    cout << "Choisissez votre déplacement [1-6] : ";
    cin >> input;
    if (cin.fail()) { // In case the user doesn't type an int
        cin.clear();
        cin.ignore(INT32_MAX, '\n');
    }
    cout << endl;

    return doMove(*m_p[0], i, j, input, 0);
}


/**
 * Asks user to input an action according to choices
 * previously displayed (displayPossibleActions())
 * There are 5 different actions
 * @return true Movement action which ends a turn
 * @return false Any other action
 */
bool Partie::askUserAction()
{
    int input;
    cout << "Choisissez votre action [1-7] : ";
    cin >> input;
    if (cin.fail()) { // In case the user doesn't type an int
        cin.clear();
        cin.ignore(INT32_MAX, '\n');
    }
    cout << endl;

    if (input == 1) // Use Item
        askUserUseItem();
    else if (input == 2) // Pick Item
        askUserPickItem();
    else if (input == 3) // Put Item
        askUserPutItem();
    else if (input == 4) // Move (will end the turn)
        return askUserMove();
    else if (input == 5) // display MiniMap
        miniMap();
    else if (input == 6) // display player information
        cout << *m_p[0] << endl;
    else if (input == 7) // Save the game
        saveGameToFile();
    else // Action not recognized
        cout << "Action non reconnue." << endl;

    return false;
}

/**
 * Starts an automatic duel between 2 Personnages
 * They fight to death and stop only when one is dead
 * @param p1 Personnage 1
 * @param p2 Personnage 2
 */
void Partie::duel(Personnage &p1, Personnage &p2)
{
    cout << "DUEL ENTRE : " << p1.getName() << " et " << p2.getName() << endl;
    while (!p1.isDead() && !p2.isDead())
    {
        p1.attack(p2);
        if (!p2.isDead())
            p2.attack(p1);
    }
    cout << "*************** FIN DU DUEL ***************" << endl << endl;

    // Add 1 score to winner
    int idx = -1;
    for (size_t i = 0; i < m_p.size(); i++) {
        if (!p1.isDead()) { // P1 won
            if (m_p[i] == &p1)
                idx = i;
        } else { // P2 won
            if (m_p[i] == &p2)
                idx = i;
        }
    }
    if (idx != -1)
        m_scores[idx]++;
}

/**
 * Searches the castle for rooms where
 * 2 or more people are gathered,
 * and make them fight each other to death.
 * The players can have a turn when there is
 * no more battles left.
 * The battles are automatic.
 */
void Partie::autoBattle()
{
    // Check all players' positions
    for (size_t i = 0; i < m_p.size() - 1; i++) {
        if (!m_p[i]->isDead()) { // Player i is alive
            vector<Personnage *> p_room; // Add people if in the same room
            p_room.push_back(m_p[i]);
            int x1 = m_pos[i].first; // Player i's coordinates
            int y1 = m_pos[i].second;

            // For all Player i != Player j
            for (size_t j = i + 1; j < m_p.size(); j++) {
                if (!m_p[j]->isDead()) { // Player j is alive
                    int x2 = m_pos[j].first; // Player j's coordinates
                    int y2 = m_pos[j].second;

                    if (x1 == x2 && y1 == y2) // Player j in the same room as Player i
                        p_room.push_back(m_p[j]);
                }
            }
            // More than 2 Players do random battles
            int nbPlayers = p_room.size();
            while ((nbPlayers = p_room.size()) > 2) {
                int rand_x = rand()%(nbPlayers); // (size - 1) + 1
                int rand_y = rand()%(nbPlayers);

                // Find Random opponent
                while (rand_y == rand_x)
                    rand_y = rand()%(nbPlayers);

                // Battle
                duel(*p_room[rand_x], *p_room[rand_y]);
                for (size_t idx = 0; idx < p_room.size(); idx++) {
                    if (p_room[idx]->isDead()) { // Remove dead people
                        p_room.erase(p_room.begin() + idx);
                        idx--;
                    }
                }
            }
            // Battle if there are exactly 2 people
            if (nbPlayers == 2) {
                duel(*p_room[0], *p_room[1]);
            }
            p_room.clear();
        }
    }
}

/**
 * Player's turn (user)
 * Actions other than Moves can be repeated
 * A turn ends only when a Move is chosen
 */
void Partie::userTurn()
{
    cout << "=============== Description de la pièce ===============" << endl;
    size_t i = m_pos[0].first; // Player's coordinates
    size_t j = m_pos[0].second;
    for (size_t k = 0; k < m_castle[i][j].first.size(); k++)
        cout << *m_castle[i][j].first[k] << endl;
    cout << "=======================================================" << endl << endl;

    cout << "--------------- Tour du Joueur ---------------" << endl;
    displayPossibleActions();
    while (!askUserAction()) // true if Move
        displayPossibleActions();
    cout << "----------------- Fin du Tour ----------------" << endl << endl;
}

/**
 * Do random actions for a Personnage
 * @param p Personnage doing the action (NPC)
 * @param i Index of the Personnage in vectors
 */
void Partie::doRandomActions(Personnage &p, size_t i)
{
    // They just move, we don't want them to get stronger
    // So we actually have a higher chance to win
    int rand_direction = 1 + (rand() % 6); // [1-6]
    bool open = false; // Random Door is open
    int x = m_pos[i].first; // Player i's coordinates
    int y = m_pos[i].second;

    while (!open) {
        if ((rand_direction == 5 && getNorthDoor(x, y))
            || (rand_direction == 3 && getEastDoor(x, y))
            || (rand_direction == 2 && getSouthDoor(x, y))
            || (rand_direction == 1 && getWestDoor(x, y))
            || rand_direction == 6) // 6 == NoMove
            open = true;
        else
            rand_direction = 1 + (rand() % 6); // [1-6]
    }

    doMove(p, m_pos[i].first, m_pos[i].second, rand_direction, i);
    cout << p.getName() << " (NPC) a joué son tour." << endl << endl;
}

/**
 * start the game
**/
void Partie::game_start()
{
    // Search for a battle when the game starts
    autoBattle();

    while (!checkFinish())
    {
        userTurn();

        for (size_t i = 1; i < m_p.size(); i++)
            if (!m_p[i]->isDead())
                doRandomActions(*m_p[i], i); // NPC's Turn

        // Automatic battle after each turn
        autoBattle();
    }
    cout << *this << endl; // End of the game
}

/**
 * init vector castle (empty)
**/
void Partie::initMap()
{
    size_t castleSize = 5; // Size of the castle (size*size)

    // Create all vectors for accessible doors
                                // Order { North, East,  South, West  }
    const vector<bool> vectTopLeft       { false, true,  true,  false };
    const vector<bool> vectTopRow        { false, true,  true,  true  };
    const vector<bool> vectTopRight      { false, false, true,  true  };
    const vector<bool> vectLeftCol       { true,  true,  true,  false };
    const vector<bool> vectMiddle        { true,  true,  true,  true  };
    const vector<bool> vectRightCol      { true,  false, true,  true  };
    const vector<bool> vectBottomLeft    { true,  true,  false, false };
    const vector<bool> vectBottomCol     { true,  true,  false, true  };
    const vector<bool> vectBottomRight   { true,  false, false, true  };

    // Empty Contents
    vector<Conteneur *> empty;

    // Create all possible empty rooms (pair of (Contents, Doors))
    pair <vector<Conteneur *>, const vector<bool>> topLeftRoom =
        make_pair(empty, vectTopLeft);
    pair <vector<Conteneur *>, const vector<bool>> topRowRoom =
        make_pair(empty, vectTopRow);
    pair <vector<Conteneur *>, const vector<bool>> topRightRoom =
        make_pair(empty, vectTopRight);
    pair <vector<Conteneur *>, const vector<bool>> leftColRoom =
        make_pair(empty, vectLeftCol);
    pair <vector<Conteneur *>, const vector<bool>> middleRoom =
        make_pair(empty, vectMiddle);
    pair <vector<Conteneur *>, const vector<bool>> rightColRoom =
        make_pair(empty, vectRightCol);
    pair <vector<Conteneur *>, const vector<bool>> BottomLeftRoom =
        make_pair(empty, vectBottomLeft);
    pair <vector<Conteneur *>, const vector<bool>> BottomRowRoom =
        make_pair(empty, vectBottomCol);
    pair <vector<Conteneur *>, const vector<bool>> BottomRightRoom =
        make_pair(empty, vectBottomRight);

    // Create vectors of rooms (rows of rooms)
    vector< pair <vector<Conteneur *>, const vector<bool>> > topRowRooms;
    vector< pair <vector<Conteneur *>, const vector<bool>> > midRowRooms;
    vector< pair <vector<Conteneur *>, const vector<bool>> > botRowRooms;
    topRowRooms.push_back(topLeftRoom);
    midRowRooms.push_back(leftColRoom);
    botRowRooms.push_back(BottomLeftRoom);
    for (size_t i = 2; i < castleSize; i++) {
        topRowRooms.push_back(topRowRoom);
        midRowRooms.push_back(middleRoom);
        botRowRooms.push_back(BottomRowRoom);
    }
    topRowRooms.push_back(topRightRoom);
    midRowRooms.push_back(rightColRoom);
    botRowRooms.push_back(BottomRightRoom);

    // Create a castle of rooms
    m_castle.clear();
    m_castle.push_back(topRowRooms);
    for (size_t i = 2; i < castleSize; i++)
        m_castle.push_back(midRowRooms);
    m_castle.push_back(botRowRooms);
}

/**
 * Add players at random positions
 * Add 20 Loots (Objet) at random positions
 * The items added are randomly picked from
 * the vector of existing items
 */
void Partie::addPlayersAndLoots()
{
    size_t castleSize = 5; // Size of the castle (size*size)
    size_t numberObjetGenerated = 20; // Number of items we randomly add in rooms

    // Add players at random location
    srand(time(NULL));
    size_t max = castleSize - 1;
    for (size_t i = 0; i < m_p.size(); i++) {
        // Generate random numbers
        int rand_x = rand()%(max + 1);
        int rand_y = rand()%(max + 1);
        m_castle[rand_x][rand_y].first.push_back(m_p[i]);
        m_pos[i].first = rand_x; // Set position of player
        m_pos[i].second = rand_y;
    }

    // Add random objects in random rooms
    size_t nbItems = Objet::getNbItems();
    size_t maxNbItems = nbItems > 0 ? nbItems - 1 : 0; // All existing Objet
    for (size_t i = 0; i < numberObjetGenerated; i++) {
        // Generate random numbers
        int rand_x = rand()%(max + 1);
        int rand_y = rand()%(max + 1);
        int rand_Objet = rand()%(maxNbItems + 1);

        Objet *obj = Objet::getItemAt(rand_Objet);
        if (obj != nullptr)
            m_castle[rand_x][rand_y].first.push_back(obj);
    }
}

/**
 * Removes a Conteneur from a Room in the castle
 * @param i Coordinate in castle
 * @param j Coordinate in castle
 * @param c Conteneur could be Personnage or Objet
 */
void Partie::removeContentFromRoom(size_t i, size_t j, Conteneur *c)
{
    if (i >= 0 && j >= 0 && i < m_castle.size() && j < m_castle[i].size() && c != nullptr)
    {
        int idx = -1; // Index of the Conteneur in vector
        for (size_t k = 0; k < m_castle[i][j].first.size(); k++)
            if (c == m_castle[i][j].first[k])
                idx = k;

        if (idx != -1) // If found
            m_castle[i][j].first.erase(m_castle[i][j].first.begin() + idx);
    }
}

/**
 * Checks if the Player is alive (Not the NPCs)
 * @return true Player is alive
 * @return false Player is dead
 */
bool Partie::isPlayerAlive() const
{
    if (m_p.size() <= 0)
        return false;
    return !m_p[0]->isDead();
}

/**
 * Get position of player of Index i in the vector
 * @param i Index of the player we want its position
 * @return pair<int, int> Position in the grid
 */
pair<int, int> Partie::getPlayerPosition(size_t i) const
{
    if (i < 0 || i >= m_pos.size())
        return make_pair(-1, -1);
    return m_pos[i];
}

/**
 * @return int: number of door opened by player1 p1
**/
int Partie::getNbDoorP1() const
{
    return m_nbDoors[0];
}

/**
 * @return int: number of door opened by player2 p2
**/
int Partie::getNbDoorP2() const
{
    return m_nbDoors[1];
}

/**
 * @return int: number of door opened by player3 p3
**/
int Partie::getNbDoorP3() const
{
    return m_nbDoors[2];
}


/**
 * @return int: number of door opened by player4 p4
**/
int Partie::getNbDoorP4() const
{
    return m_nbDoors[3];
}


/**
 * @return float : get player1 p1 's score
**/
float Partie::getScoreP1() const
{
    return m_scores[0];
}

/**
 * @return float : get player2 p2 's score
**/
float Partie::getScoreP2() const
{
    return m_scores[1];
}

/**
 * @return float : get player3 p3 's score
**/
float Partie::getScoreP3() const
{
    return m_scores[2];
}

/**
 * @return float : get player 4 p4 's score
**/
float Partie::getScoreP4() const
{
    return m_scores[3];
}

vector<int> Partie::getNbDoors() const
{
    return m_nbDoors;
}

vector<float> Partie::getScores() const
{
    return m_scores;
}

vector<string> Partie::getNames() const
{
    vector<string> names;
    for (auto p : m_p)
        names.push_back(p->getName());
    return names;
}

/**
 * Get the North Door in a Castle Room
 *
 * @return true : the door is open
 * @return false : the door is closed
 */
bool Partie::getNorthDoor(size_t i, size_t j) const
{
    if (i < 0 || j < 0 || i >= m_castle.size() || j >= m_castle[i].size())
        return false;
    return m_castle[i][j].second[0];
}

/**
 * Get the East Door in a Castle Room
 *
 * @return true : the door is open
 * @return false : the door is closed
 */
bool Partie::getEastDoor(size_t i, size_t j) const
{
    if (i < 0 || j < 0 || i >= m_castle.size() || j >= m_castle[i].size())
        return false;
    return m_castle[i][j].second[1];
}

/**
 * Get the South Door in a Castle Room
 *
 * @return true : the door is open
 * @return false : the door is closed
 */
bool Partie::getSouthDoor(size_t i, size_t j) const
{
    if (i < 0 || j < 0 || i >= m_castle.size() || j >= m_castle[i].size())
        return false;
    return m_castle[i][j].second[2];
}

/**
 * Get the West Door in a Castle Room
 *
 * @return true : the door is open
 * @return false : the door is closed
 */
bool Partie::getWestDoor(size_t i, size_t j) const
{
    if (i < 0 || j < 0 || i >= m_castle.size() || j >= m_castle[i].size())
        return false;
    return m_castle[i][j].second[3];
}

/**
 * display vector castle
**/
void Partie::displayCastle() const
{
    cout << "********** Château **********" << endl << endl;
    for (size_t i = 0; i < m_castle.size(); i++) {
        cout << "          ***** [Ligne " << i << "] *****" << endl;
        for (size_t j = 0; j < m_castle[i].size(); j++) {
            cout << "Pièce [" << i << ", " << j << "] :" << endl;
            cout << "====================" << endl;

            cout << "[Portes disponibles] : {";
            if (getNorthDoor(i, j))
                cout << " Nord ";
            if (getEastDoor(i, j))
                cout << " Est ";
            if (getSouthDoor(i, j))
                cout << " Sud ";
            if (getWestDoor(i, j))
                cout << " Ouest";
            cout << " }" << endl;

            for (size_t k = 0; k < m_castle[i][j].first.size(); k++)
                cout << *m_castle[i][j].first[k] << endl;
            cout << "====================" << endl << endl;
        }
    }
    cout << "*****************************" << endl << endl;
}

/**
 * Display vector players
 */
void Partie::displayPlayers() const
{
    cout << "******* Liste des joueurs *******" << endl;
    for (size_t i = 0; i < m_p.size(); i++)
        cout << *m_p[i] << endl;
    cout << "*********************************" << endl << endl;
}

/**
 * Minimap showing castle layout and players positions
 * We don't know which NPC is at the 'x' position
 * We don't show dead people
 * Player is always number 0, and NPCs > 0
 * Player displayed as P
 * NPCs as N
 * Multiple people in a room as M
 */
void Partie::miniMap() const
{
    cout << "Mini Carte" << endl;
    cout << "---------------------" << endl;
    for (size_t i = 0; i < m_castle.size(); i++) {
        for (size_t j = 0; j < m_castle[i].size(); j++) {
            int cpt = 0;
            int isPlayer = 0;
            for (size_t k = 0; k < m_castle[i][j].first.size(); k++) {
                if (instanceof<Personnage>(m_castle[i][j].first[k])) {
                    Personnage *p = dynamic_cast<Personnage *>(m_castle[i][j].first[k]);
                    if (m_p[0] == p)
                        isPlayer = 1;
                    if (!p->isDead()) // Don't show dead people except User
                        cpt++;
                }
            }
            if (isPlayer == 1 && cpt == 1)
                cout << "| P ";
            else if (isPlayer == 0 && cpt == 1)
                cout << "| N ";
            else if (cpt > 1)
                cout << "| M ";
            else
                cout << "|   ";
        }
        cout << "|" << endl;
        cout << "---------------------" << endl;
    }
}

/**
 * Call : instanceof<Class *>(ptrToCheck);
 * @return bool : INSTANCE OF
**/
template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
    return dynamic_cast<const Base*>(ptr) != nullptr;
}

Partie::~Partie() {}

// Extension File

/**
 * @brief Saves the current game (mannual save) into a file
 * We suppose the saved file is not touched outside of this program
 */
void Partie::saveGameToFile()
{
    ofstream outFile ("gameData.txt"); // Open file

    // Save player position as "pos1 pos2 "
    for (size_t i = 0; i < m_pos.size(); i++) {
        pair<int, int> position = getPlayerPosition(i);
        outFile << position.first << " " << position.second << " ";
    }
    outFile << endl;

    // Save m_nbDoors as "nb "
    for (size_t i = 0; i < m_nbDoors.size(); i++)
        outFile << m_nbDoors[i] << " ";
    outFile << endl;

    // Save m_scores as "score "
    for (size_t i = 0; i < m_scores.size(); i++)
        outFile << m_scores[i] << " ";
    outFile << endl;

    // Save m_p as "nom\n className\n currentHP\n idxObj1 idxObj2 idxObj3 idxObj4"
    for (size_t i = 0; i < m_p.size(); i++) {
        outFile << m_p[i]->getName() << endl;
        outFile << m_p[i]->getClassName() << endl;
        outFile << m_p[i]->getHealth() << endl;
        vector<int> bagItemIndexes = m_p[i]->getBagItemIndexes(); // Size always 4
        for (auto item : bagItemIndexes)
            outFile << item << " "; // Idx can be -1, it represents noItem
        outFile << endl;
    }

    // Save m_castle as
    /*
    "R\n R\n R\n R\n R\n"
    ...
    "R\n R\n R\n R\n R\n"

    with each R as
    "R: c0 ; c1 ; c2 ; ..." // contents of a room
     */
    for (size_t i = 0; i < m_castle.size(); i++) {
        for (size_t j = 0; j < m_castle[i].size(); j++) {
            int cpt = 0;
            for (size_t k = 0; k < m_castle[i][j].first.size(); k++) {
                // We don't store Personnage in rooms, only Objet
                if (instanceof<Objet>(m_castle[i][j].first[k])) {
                    Objet *o = dynamic_cast<Objet *>(m_castle[i][j].first[k]);
                    outFile << Objet::getObjetIndex(o) << " ";
                    cpt++;
                }
            }
            if (cpt == 0)
                outFile << "-1 "; // Add -1 if room has no Objet
            outFile << endl;
        }
        outFile << endl;
    }


    outFile.close(); // Close file
    cout << "Partie sauvegardée." << endl;
}
