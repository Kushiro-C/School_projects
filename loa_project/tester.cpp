#include "Partie.hpp"
#include "Conteneur.hpp"

// tests
int main()
{
    // Test Personnages
    Conteneur *pA = Personnage::factory("nom_Amazone", "amazone");
    Conteneur *pG = Personnage::factory("nom_Guerrier", "guerrier");
    Conteneur *pM = Personnage::factory("nom_Moine", "moine");
    Conteneur *pS = Personnage::factory("nom_Sorciere", "sorciere");
    Conteneur *pDefault = Personnage::factory("nomDefautGuerrier", "default");

    // Test print correct names for Personnages
    cout << "********** TEST AFFICHAGE PERSONNAGES **********" << endl;
    cout << "ATTENDU : (1-5) PERSONNAGES DETAILS" << endl << endl;
    cout << "(1) " << *pA;
    cout << "(2) " << *pG;
    cout << "(3) " << *pM;
    cout << "(4) " << *pS;
    cout << "(5) " << *pDefault << endl;

    // Test Objets
    Conteneur *oB = Objet::factory("nomObj_Bouteile", 'b', 0, 10.5, "Desc_Bouteille");
    Conteneur *oE = Objet::factory("nomObj_Equipement", 'e', 0, 20, "Desc_Equip");
    Conteneur *oC = Objet::factory("nomObj_Cle", 'k', 1, 0);
    Conteneur *oNullptr = Objet::factory("nomObj_Nullptr", 'k', 0, 0, "Not Cle");
    Conteneur *oNullptr2 = Objet::factory("nomObj_Nullptr2", 'b', 2, 3);

    // Test print correct names for Objets
    cout << "********** TEST AFFICHAGE OBJETS **********" << endl;
    cout << "ATTENDU : (1-3) OBJETS DETAILS, (4-5) NULLPTR" << endl << endl;
    cout << "(1) " << *oB;
    cout << "(2) " << *oE;
    cout << "(3) " << *oC;
    if (oNullptr != nullptr)
        cout << "(4) NOT NULLPTR, BUT SHOULD BE NULLPTR" << endl;
    else
        cout << "(4) NULLPTR OK" << endl;
    if (oNullptr2 != nullptr)
        cout << "(5) NOT NULLPTR, BUT SHOULD BE NULLPTR" << endl;
    else
        cout << "(5) NULLPTR OK" << endl;

    // Test display inventory
    cout << endl << "********** TEST AFFICHAGE INVENTAIRE **********" << endl;
    cout << "ATTENDU : (1) STARTER EQUIPMENT" << endl
        << "(2) FULL MSG, THEN ITEMS 4, (3) DESC PERSONNAGE STATS UP" << endl
        << "(4) ITEM 2, (5) DESC PERSONNAGE STATS REDUCED" << endl << endl;
    cout << "(1)" << endl;
    pDefault->printBag();

    // Test equip item
    cout << "(2)" << endl;
    Objet *objE = dynamic_cast<Objet *>(oE); // cast
    // pDefault has one starterItem
    pDefault->addItemToBag(objE); // Adding 1 Equipment
    Objet *objB = dynamic_cast<Objet *>(oB); // cast
    pDefault->addItemToBag(objB); // Adding 2 Bouteilles
    pDefault->addItemToBag(objB);
    pDefault->addItemToBag(objB); // Full message
    pDefault->printBag(); // 4 items
    cout << "(3) " << *pDefault << endl; // HP 19020, AP 170 ((POWER_SHIELD=20) + starter)

    // Test unequip item
    cout << "(4)" << endl;
    pDefault->removeItemFromBag(objE);
    pDefault->removeItemFromBag(objE); // No change
    pDefault->removeItemFromBag(objB);
    pDefault->printBag(); // 1 Bouteille left and Start Equipment
    cout << "(5) " << *pDefault << endl; // HP 19000, AP 170 (remove shield -20AP)

    // Test Init a map for a Partie
    cout << "********** TEST AFFICHAGE PARTIE **********" << endl;
    cout << "ATTENDU : (1) LISTE PERSONNAGES, " << endl
        << "(2) CHATEAU (RANDOM LOOTS AND PLAYERS POSITION)" << endl
        << "(3) STATUT PARTIE AVEC SCORES" << endl
        << "(4) MINIMAP" << endl << endl;
    Personnage *persA = Personnage::factory("nom_Amazone", "amazone");
    Personnage *persG = Personnage::factory("nom_Guerrier", "guerrier");
    Personnage *persM = Personnage::factory("nom_Moine", "moine");
    Personnage *persS = Personnage::factory("nom_Sorciere", "sorciere");
    Partie partie {*persA, *persG, *persM, *persS};

    cout << "(1) LISTE PERSONNAGES" << endl;
    partie.displayPlayers();
    cout << "(2) CHATEAU (RANDOM)" << endl;
    partie.displayCastle(); // BIG
    cout << "(3) STATUT PARTIE" << endl;
    cout << partie << endl;
    cout << "(4) MINIMAP" << endl;
    partie.miniMap();

    return 0;
}
