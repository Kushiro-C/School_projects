#ifndef GUERRIER_HPP
#define GUERRIER_HPP

#include "Personnage.hpp"

/**
 * Guerrier is a well rounded class with both
 * good HP and good ability.
 */
class Guerrier : public Personnage
{

private :
    Guerrier(const string name); //constrcutor
    friend class Personnage;
    Guerrier(const Guerrier &a) = delete; //copy constructor
    Guerrier& operator=(Guerrier const&) = delete; //operator =

public:
    friend ostream& operator<<(ostream& out, Guerrier& x);
    void printCharacter(ostream& where) const; // From Personnage
    double special_effect(Personnage &target);
    string getClassName();
    virtual ~Guerrier(); // virtual desctructor
};

#endif /* Guerrier_HPP */

