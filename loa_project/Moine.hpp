#ifndef MOINE_HPP
#define MOINE_HPP

#include "Personnage.hpp"

/**
 * A Moine has a lot of HP, but low ability.
 */
class Moine : public Personnage
{

private :
    Moine(const string name); //constrcutor
    friend class Personnage;
    Moine(const Moine &a) = delete; //copy constructor
    Moine& operator=(Moine const&) = delete; //operator =

public:
    friend ostream& operator<<(ostream& out, Moine& x);
    void printCharacter(ostream& where) const; // From Personnage
    double special_effect(Personnage &target);
    string getClassName();
    virtual ~Moine(); // virtual desctructor
};

#endif /* Moine_HPP */

