#ifndef AMAZONE_HPP
#define AMAZONE_HPP

#include "Personnage.hpp"

/**
 * An Amazone has low HP but high ability.
 */
class Amazone : public Personnage
{

private :
    Amazone(const string name); //constrcutor
    friend class Personnage;
    Amazone(const Amazone &a) = delete; //copy constructor
    Amazone& operator=(Amazone const&) = delete; //operator =

public:
    friend ostream& operator<<(ostream& out, Amazone& x);
    void printCharacter(ostream& where) const; // From Personnage
    double special_effect(Personnage &target);
    string getClassName();
    virtual ~Amazone(); // virtual desctructor
};

#endif /* Amazone_HPP */

