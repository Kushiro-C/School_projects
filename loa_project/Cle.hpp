#ifndef CLE_HPP
#define CLE_HPP

#include "Objet.hpp"

/**
 * Teleport Key
 */
class Cle : public Objet
{

private :
    const int m_x;  // position x for teleportation
    const int m_y; // position y for teleportation
    Cle(const string name, const int x, const int y); // constrcutor
    friend class Objet;
    Cle(const Cle &a) = delete; //copy constructor
    Cle& operator=(Cle const&) = delete; //operator =

public:
    friend ostream& operator<<(ostream& out, Cle& x);
    void printObjet(ostream& where) const; // from Objet
    int getX() const; // key position x
    int getY() const; // key position y
    virtual ~Cle(); // virtual desctructor
};

#endif /* Cle_HPP */

