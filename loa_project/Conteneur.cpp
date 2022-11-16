#include "Conteneur.hpp"

Conteneur::Conteneur()
{
}

Conteneur::~Conteneur()
{
}

/**
 * operator print for Conteneur
**/
ostream& operator<<(std::ostream& out, Conteneur& x) {
    x.print(out);
    return out;
}

/**
 * Prints information instead of operator
 * We use this auxiliary method as a virtual
 * to call child classes
 */
void Conteneur::print(std::ostream& where) const {
    where << "Conteneur" << endl;
}

/**
 * Calls child method from Personnage
 */
void Conteneur::printBag() {}

/**
 * Calls child method from Personnage
 */
void Conteneur::addItemToBag(Objet *item) {}

/**
 * Calls child method from Personnage
 */
void Conteneur::removeItemFromBag(Objet *item) {}