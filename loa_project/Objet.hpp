#ifndef OBJET_HPP
#define OBJET_HPP

#include "Conteneur.hpp"

class Personnage;

class Objet : public Conteneur
{

protected:
    const string m_name; // name
    const char m_type; // type // b:bottle, e:equipments, k:keys
    const string m_desc; // description
    Objet(const string name, const char type, const string desc); // constrcutor
    static const vector<Objet *> m_allExistingItems; // all items that could be in the game
    // starter items for different classes
    static const vector<Objet *> m_starterAmazone;
    static const vector<Objet *> m_starterGuerrier;
    static const vector<Objet *> m_starterMoine;
    static const vector<Objet *> m_starterSorciere;

    friend class Personnage;
    Objet(const Objet &a) = delete; //copy constructor
    Objet& operator=(Objet const&) = delete; //operator =

public:
    static Objet *factory(const string name, const char type, // For all except Cle
                    const int child_type, const double power, const string desc);
    static Objet *factory(const string name, const char type, // For Cle
                        const int x, const int y);
    static int getObjetIndex(Objet *obj);
    static Objet *getItemAt(size_t i);
    static size_t getNbItems();
    static vector<Objet *> getStarterAmazone();
    static vector<Objet *> getStarterGuerrier();
    static vector<Objet *> getStarterMoine();
    static vector<Objet *> getStarterSorciere();

    friend ostream& operator<<(ostream& out, Objet& x);
    void print(ostream& where) const; // from Conteneur
    virtual void printObjet(ostream& where) const; // call Objet Child

    string getName() const; // get name
    char getType() const; // get type
    string getDesc() const; // get description
    virtual ~Objet(); //virtual desctructor

    void printBag();
    void addItemToBag(Objet *item);
    void removeItemFromBag(Objet *item);

};

#endif /* Objet_HPP */

