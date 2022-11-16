#ifndef BOUTEILLE_HPP
#define BOUTEILLE_HPP

#include <iostream>
#include <string>
using namespace std;
#include <vector>
#include <cmath>
#include "Objet.hpp"
#include "Personnage.hpp"

class Bouteille : public Objet
{

private :
    const int m_bouteille_type; // 0 poison or 1 healing
    const double m_puissance; // puissance
    Bouteille(const string name, const int bouteille_type,
            const double puissance, const string desc); // constrcutor
    friend class Objet;
    Bouteille(const Bouteille &a) = delete; //copy constructor
    Bouteille& operator=(Bouteille const&) = delete; //operator =

public:
    friend ostream& operator<<(ostream& out, Bouteille& x);
    void printObjet(ostream& where) const; // from Objet
    int getBouteille_type() const; //get Bouteille type
    double getPuissance() const; //get puissance
    void use(Personnage &target); //use the bottle
    virtual ~Bouteille(); // virtual desctructor
};

#endif /* Bouteille_HPP */

