#include "Bouteille.hpp"

/**
 * constructor
**/
Bouteille::Bouteille(const string name, const int bouteille_type,
                    const double puissance, const string desc)
: Objet(name, 'b', desc),
m_bouteille_type(bouteille_type), m_puissance(puissance)
{
}

/**
 * @return int 0 for poison, 1 for healing
**/
int Bouteille::getBouteille_type() const
{
    return m_bouteille_type;
}

/**
 * @return double m_puissance
**/
double Bouteille::getPuissance() const
{
    return m_puissance;
}

/**
 * use the potion/poison on target
 * don't forget to delete the bottle after use
 * If the bottle is a Poison type, we remove it from the user (in Partie)
**/
void Bouteille::use(Personnage &target)
{
    if (m_bouteille_type == 0) // poison
        target.setHealth(target.getHealth() - m_puissance);
    if (m_bouteille_type == 1) { // healing
        target.setHealth(target.getHealth() + m_puissance);
        target.removeItemFromBag(this);
    }
}

Bouteille::~Bouteille()
{
}

/**
 * display bottle
**/
ostream& operator<<(ostream& out, Bouteille& x)
{
    x.printObjet(out);
    return out;
}

/**
 * Called from parent Objet
 */
void Bouteille::printObjet(ostream &where) const {
    where << "Objet : " << m_name << endl
        << "Type : " << m_type << endl;
    if (m_bouteille_type == 0)  // Poison
        where << "Poison de puissance " << m_puissance << endl;
    else                        // Soin
        where << "Soin de puissance " << m_puissance << endl;
    where << "Description : " << m_desc << endl;
}