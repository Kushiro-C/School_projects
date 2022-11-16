#ifndef EQUIPEMENT_HPP
#define EQUIPEMENT_HPP

#include "Objet.hpp"
#include "Personnage.hpp"

class Equipement : public Objet
{

private :
    const int m_equipement_type; // 0 shield or 1 weapon
    const double m_puissance; //puissance
    Equipement(const string name, const int equipement_type,
                const double puissance, const string desc); // constrcutor
    friend class Objet;
    Equipement(const Equipement &a) = delete; //copy constructor
    Equipement& operator=(Equipement const&) = delete; //operator =

public:
    friend ostream& operator<<(ostream& out, Equipement& x);
    void printObjet(ostream& where) const; // from Objet
    int getEquipement_type() const; //get Equipement type
    double getPuissance() const; //get puissance
    void equip(Personnage &target); // equip equipment
    void unEquip(Personnage &target); // unequip equipment
    virtual ~Equipement(); // virtual desctructor
};

#endif /* Equipement_HPP */

