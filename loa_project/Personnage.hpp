#ifndef PERSONNAGE_HPP
#define PERSONNAGE_HPP

#include "Objet.hpp"

class Amazone;
class Guerier;
class Moine;
class Sorciere;
class Bouteille;
class Equipement;
class Cle;

class Personnage : public Conteneur
{
private :
    const string m_name; // name
    double m_health; // health (current)
    double m_ability; // hability (AP)
    double m_maxHP; // max health
    vector<Objet *> m_bag; // bag 4 objects
    Personnage(const string name, const double health, const double ability); // constrcutor

    friend class Amazone;
    friend class Guerrier;
    friend class Moine;
    friend class Sorciere;
    friend class Bouteille;
    friend class Equipement;
    friend class Cle;

    Personnage(const Personnage &a) = delete; //copy constructor
    Personnage& operator=(Personnage const&) = delete; //operator =

public:
    static Personnage *factory(const string name, const string className);
    virtual double special_effect(Personnage &target); // character special effect calcul a double
    void attack(Personnage &target);
    bool isDead() const; // bool if dead

    virtual string getClassName();
    vector<int> getBagItemIndexes() const;
    string getName() const; // get name
    double getHealth() const; // get hp
    double getAbility() const; // get ability
    vector<Objet *> getBag() const; // get bag
    void setHealth(double health); // set hp
    void setAbility(double ability); // set ability
    friend ostream& operator<<(ostream& out, Personnage& x);
    void print(ostream& where) const; // from Conteneur

    virtual void printCharacter(ostream& where) const; // call Personnage Child
    void printBag() const;
    void addItemToBag(Objet *item);
    void removeItemFromBag(Objet *item);
    bool isBagFull() const;
    bool hasItemInBag(Objet *obj) const;
    bool hasPotionInBag() const;
    virtual ~Personnage(); // virtual desctructor
};

#endif /* Personnage_HPP */

