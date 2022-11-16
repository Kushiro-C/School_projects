#ifndef SORCIERE_HPP
#define SORCIERE_HPP

#include "Personnage.hpp"

/**
 * A Sorciere has low HP, pretty high ability.
 */
class Sorciere : public Personnage
{

private :
    Sorciere(const string name); //constrcutor
    friend class Personnage;
    Sorciere(const Sorciere &a) = delete; //copy constructor
    Sorciere& operator=(Sorciere const&) = delete; //operator =

public:
    friend ostream& operator<<(ostream& out, Sorciere& x);
    void printCharacter(ostream& where) const; // From Personnage
    double special_effect(Personnage &target);
    string getClassName();
    virtual ~Sorciere(); // virtual desctructor
};

#endif /* Sorciere_HPP */

