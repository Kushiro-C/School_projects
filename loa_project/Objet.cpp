#include "Objet.hpp"
#include "Bouteille.hpp"
#include "Equipement.hpp"
#include "Cle.hpp"

/**
 * Here we have all Items created for this game
 * which will then be added in m_allExistingItems
 */
// Clés
Objet *cle = Objet::factory("Clé de téléportation du château", 'k', 2, 2);
// Position (-1,-1) for random position in castle
Objet *cleSombre = Objet::factory("Clé de téléportation sombre", 'k', -1, -1);

// Bouteilles
Objet *bouteillePoison = Objet::factory("Potion de poison", 'b', 0, 200,
    "Potion qui inflige une petite blessure à l'ennemi.");
Objet *bouteilleSoin = Objet::factory("Potion de soin", 'b', 1, 200,
    "Potion qui soigne légèrement les blessures.");
Objet *poisonFort = Objet::factory("Potion de poison FORT", 'b', 0, 2000,
    "Potion dangereuse qui inflige de lourds dégâts.");
Objet *soinFort = Objet::factory("Potion de soin FORT", 'b', 1, 2500,
    "Potion qui soigne les dégâts internes sévères.");
Objet *poisonOP = Objet::factory("Potion de poison OP", 'b', 0, 5000,
    "Potion mortelle conçue par un grand alchimiste maléfique.\n"
    "Cette potion a été scellée à cause de sa puissance démesurée.");
Objet *soinOP = Objet::factory("Potion de soin OP", 'b', 1, 7000,
    "Potion bénie par les esprits et par le peuple, "
    "conçue avec tout le savoir d'un grand alchimiste.\n"
    "Il paraît que cette potion contient de l'eau de jouvence.");

// Starter Items
Objet *starterAmazone = Objet::factory("Arc Résistant", 'e', 1, 70,
    "Un arc forgé avec un bois très résistant.");
Objet *starterGuerrier = Objet::factory("Épée Longue", 'e', 1, 50,
    "L'arme pour les novices de la chevalerie.");
Objet *starterMoine = Objet::factory("Tenue du Moine", 'e', 0, 2000,
    "Cette tenue est légère mais résistante.");
Objet *starterSorciere = Objet::factory("Baguette Magique", 'e', 1, 60,
    "Baguette donnée aux diplômés de l'institution de magie.");

// Other Equipements
Objet *arme1 = Objet::factory("Gants de boxe en fer", 'e', 1, 100,
    "Se faire cogner par ça... ça doit faire mal...");
Objet *armure1 = Objet::factory("Cape de Magicien", 'e', 0, 1200,
    "Crée une sorte d'illusion qui déconcentre l'ennemi.");
Objet *arme2 = Objet::factory("Râteau Maudit", 'e', 1, 150,
    "Légende raconte que plusieurs génération de paysans l'utilisait...");
Objet *armure2 = Objet::factory("Bouclier en acier", 'e', 0, 1750,
    "Protège de la plupart des coups simples.");
Objet *arme3 = Objet::factory("Claymore", 'e', 1, 190,
    "Épée écossaise longue et dure, le rêve de tout homme.");
Objet *armure3 = Objet::factory("Gillet pare-balles magique", 'e', 0, 2500,
    "Un Gillet pare-balles renforcé par de la magie.");
Objet *arme4 = Objet::factory("Lance-flammes", 'e', 1, 280,
    "Pourquoi la magie quand on a un Lance-flammes ?");
Objet *armure4 = Objet::factory("Bague du sixième sens", 'e', 0, 3200,
    "Procure un sixième sens face au danger direct.");

// static member
const vector<Objet *> Objet::m_allExistingItems
{
    cle, cleSombre, bouteilleSoin, soinFort, soinOP,
    bouteillePoison, poisonFort, poisonOP,
    starterAmazone, starterGuerrier, starterMoine, starterSorciere,
    arme1, arme2, arme3, arme4,
    armure1, armure2, armure3, armure4

};
const vector<Objet *> Objet::m_starterAmazone     {starterAmazone};
const vector<Objet *> Objet::m_starterGuerrier    {starterGuerrier};
const vector<Objet *> Objet::m_starterMoine       {starterMoine};
const vector<Objet *> Objet::m_starterSorciere    {starterSorciere, cleSombre};

/**
 * constructor
**/
Objet::Objet(const string name, const char type, const string desc)
    : m_name(name), m_type(type), m_desc(desc)
{
}

/**
 * Creates Objets, we use this instead of direct Constructors
 * This factory creates all Objects except Cle
 *
 * @param name Name of the Objet
 * @param type m_type in Objet
 * @param child_type m_bottle_type in Bouteille,
 *                  or m_equipement_type in Equipement
 * @param power Amount of stats it will affect (HP or AP)
 * @param desc Description of the objet
 * @return Objet* Created Objet
 */
Objet* Objet::factory(const string name, const char type,
            const int child_type, const double power, const string desc)
{
    Objet *obj = nullptr;

    if (type == 'b')
        obj = new Bouteille(name, child_type, power, desc);
    else if (type == 'e')
        obj = new Equipement(name, child_type, power, desc);

    return obj;
}

/**
 * Creates Objets, we use this instead of direct Constructors
 * This factory only creates a Cle
 *
 * @param name Name of the Cle
 * @param type m_type in Objet
 * @param x Position x
 * @param y Position y
 * @return Objet* Created Objet (Cle)
 */
Objet* Objet::factory(const string name, const char type,
                const int x, const int y)
{
    if (type == 'k')
        return new Cle(name, x, y);
    return nullptr;
}

/**
 * Get the index of an Objet in m_allExistingItems
 * If not found, it returns -1
 * @param obj Objet to get index
 * @return int Index of Objet in m_allExistingItems
 */
int Objet::getObjetIndex(Objet *obj)
{
    for (size_t i = 0; i < m_allExistingItems.size(); i++) {
        if (m_allExistingItems[i] == obj)
            return i;
    }
    return -1;
}

/**
 * Returns Objet at index i in allExistingItems
 */
Objet* Objet::getItemAt(size_t i)
{
    if (i < 0 || i >= m_allExistingItems.size())
        return nullptr;
    return m_allExistingItems[i];
}

/**
 * Returns the number of existing items
 */
size_t Objet::getNbItems()
{
    return m_allExistingItems.size();
}

/**
 * Getter starter items for class Amazone
 */
vector<Objet *> Objet::getStarterAmazone()
{
    return m_starterAmazone;
}
/**
 * Getter starter items for class Guerrier
 */
vector<Objet *> Objet::getStarterGuerrier()
{
    return m_starterGuerrier;
}
/**
 * Getter starter items for class Moine
 */
vector<Objet *> Objet::getStarterMoine()
{
    return m_starterMoine;
}
/**
 * Getter starter items for class Sorciere
 */
vector<Objet *> Objet::getStarterSorciere()
{
    return m_starterSorciere;
}

/**
 * operator<< to display
**/
ostream& operator<<(ostream& out, Objet& x)
{
    x.printObjet(out);
    return out;
}

/**
 * Called from wrapper Conteneur
 */
void Objet::print(ostream &where) const {
    printObjet(where);
}

/**
 * Calls Objet Child to print details
 */
void Objet::printObjet(ostream &where) const {
    where << "Objet : " << m_name << endl
        << "Type : " << m_type << endl
        << "Description : " << m_desc << endl;
}

string Objet::getName() const
{
    return m_name;
}

/**
 * @return char 'b' for bottle, 'e' for equipment, 'k' for key
 */
char Objet::getType() const
{
    return m_type;
}

string Objet::getDesc() const
{
    return m_desc;
}

Objet::~Objet()
{
}

/**
 * Methods inherited from Conteneur, but only defined in Personnage
 */
void Objet::printBag() {}
void Objet::addItemToBag(Objet *item) {}
void Objet::removeItemFromBag(Objet *item) {}
