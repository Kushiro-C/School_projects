#ifndef CONTENEUR_HPP
#define CONTENEUR_HPP

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

class Objet;

/**
 * Class Wrapping Objet and Personnage,
 * It represents an entity in a room.
 */
class Conteneur
{

protected :
    Conteneur(); //constrcutor
    Conteneur(const Conteneur &a) = delete; //copy constructor
    Conteneur& operator=(Conteneur const&) = delete; //operator =

public :
    friend ostream& operator<<(ostream& out, Conteneur& x);
    virtual void print(ostream& where) const;
    virtual void printBag(); // call Personnage child
    virtual void addItemToBag(Objet *item); // call Personnage child
    virtual void removeItemFromBag(Objet *item); // call Personnage child

    virtual ~Conteneur(); // virtual desctructor
};

#endif /* Conteneur_HPP */

