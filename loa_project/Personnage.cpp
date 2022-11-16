#include "Personnage.hpp"
#include "Amazone.hpp"
#include "Guerrier.hpp"
#include "Moine.hpp"
#include "Sorciere.hpp"
#include "Equipement.hpp"
#include "Bouteille.hpp"

/**
 * constructor
**/
Personnage::Personnage(const string name, const double health, const double ability)
    : m_name(name), m_health(health), m_ability(ability), m_maxHP(health)
{
}

/**
 * Calls child method
 * @return string the class name of the child
 * {"amazone", "guerrier", "moine", "sorciere"}
 */
string Personnage::getClassName()
{
    return "None";
}

/**
 * Get Indexes of the items in m_bag
 * Indexes are from Objet::m_allExistingItems
 * The size of the vector is always 4 (size of the m_bag)
 * A slot is described as -1 if there is no item
 * @return vector<int> Indexes of bag item
 */
vector<int> Personnage::getBagItemIndexes() const
{
    vector<int> itemIndexes {-1, -1, -1, -1}; // Empty bag
    for (size_t i = 0; i < m_bag.size(); i++) {
        for (size_t j = 0; j < Objet::m_allExistingItems.size(); j++) {
            Objet::getObjetIndex(m_bag[i]);
            if (Objet::m_allExistingItems[j] == m_bag[i])
                itemIndexes[i] = j;
        }
    }

    return itemIndexes;
}

/**
 * @return double m_health
**/
double Personnage::getHealth() const
{
    return m_health;
}

/**
 * @return double m_ability
**/
double Personnage::getAbility() const
{
    return m_ability;
}

/**
 * @return string m_name
**/
string Personnage::getName() const
{
    return m_name;
}

/**
 * Creates characters, we use this instead of direct Constructors
 */
Personnage* Personnage::factory(const string name, const string className)
{
    Personnage *p;

    if (className == "amazone")
        p = new Amazone(name);
    else if (className == "guerrier")
        p = new Guerrier(name);
    else if (className == "moine")
        p = new Moine(name);
    else if (className == "sorciere")
        p = new Sorciere(name);
    else // Default class (Guerrier)
        p = new Guerrier(name);

    return p;
}

/**
 * operator print for Personnage
**/
ostream& operator<<(ostream& out, Personnage& x) {
    x.printCharacter(out);
    return out;
}

/**
 * Called from wrapper Conteneur
 */
void Personnage::print(ostream &where) const {
    printCharacter(where);
}

/**
 * Calls Personnage Child to print details
 */
void Personnage::printCharacter(ostream &where) const {
    where << "HP : " << m_health << "/" << m_maxHP << "  AP : " << m_ability << endl;
}

/**
 * Prints contents of the bag
 */
void Personnage::printBag() const {
    cout << "***************" << endl;
    cout << "[Inventaire]" << endl;
    if (m_bag.empty())
        cout << "(vide)" << endl;
    else
        for (auto item : m_bag)
            cout << *item << endl;
    cout << "***************" << endl;
}

/**
 * Adds an item to a bag
 * The bag can not contain more than 4 items
 * Update stats if we add an equipment
 */
void Personnage::addItemToBag(Objet *item) {
    if (m_bag.size() < 4) {
        m_bag.push_back(item);
        if (item->getType() == 'e') { // Equipment -> update stats
            Equipement *e = dynamic_cast<Equipement *>(item);
            e->equip(*this);
        }
    } else
        cout << "Sac rempli !" << endl;
}


/**
 * Removes an item from a bag if it exists
 */
void Personnage::removeItemFromBag(Objet *item) {
    int idx = -1; // Index of the item in vector
    for (size_t i = 0; i < m_bag.size(); i++)
        if (item == m_bag.at(i))
            idx = i;

    if (idx != -1) { // If found
        if (m_bag.at(idx)->getType() == 'e') { // Equipment -> update stats
            Equipement *e = dynamic_cast<Equipement *>(m_bag.at(idx));
            e->unEquip(*this);
        }
        m_bag.erase(m_bag.begin() + idx);
    }
}

/**
 * special effect
 * @return a double, it could be used to change m_ability
 * represents character's special effect
**/
double Personnage::special_effect(Personnage &target)
{
    // See children
    return 0.0;
}


/**
 * Combat action : attack a character
**/
void Personnage::attack(Personnage &target)
{
    int rand_pot = rand()%3; // 0, 1 or 2
    if (rand_pot == 1 && hasPotionInBag()) { // 33% chance to use a potion
        for (size_t i = 0; i < m_bag.size(); i++) { // Use first potion
            if (m_bag[i]->getType() == 'b') {
                Bouteille *potion = dynamic_cast<Bouteille *>(m_bag[i]);
                cout << m_name << " a utilisé une potion de ";

                if (potion->getBouteille_type() == 1) { // Healing
                    potion->use(*this); // Use on myself
                    cout << "soin et s'est soigné " << potion->getPuissance() << " HP !" << endl;
                } else { // Poison
                    potion->use(target); // Use on target
                    cout << "poison sur " << target.m_name << " et inflige "
                        << potion->getPuissance() << " dégâts !" << endl;
                }

                removeItemFromBag(m_bag[i]); // Remove potion from bag
                return; // End attack turn
            }
        }
    } else { // 66% % chance to attack normally
        double damage = m_ability + special_effect(target);
        target.setHealth(target.m_health - damage);
        cout << m_name << " a infligé " << damage << " dégats à " << target.getName() << endl;
    }
    if (target.isDead()) {
        cout << m_name << " a tué " << target.m_name << endl;
        if (target.m_health < 0) // Negative HP
            target.m_health = 0; // Set to 0
    } else if (target.m_health < target.m_maxHP / 3) // 1/3 health
        cout << target.m_name << " est gravement blessé !" << endl;
}

/**
 * @return true if the m_bag is full (Size = 4)
 */
bool Personnage::isBagFull() const
{
    return m_bag.size() == 4;
}

/**
 * @return true Item is in the bag
 * @return false Item is not in the bag
 */
bool Personnage::hasItemInBag(Objet *obj) const
{
    for (auto o : m_bag)
        if (o == obj)
            return true;
    return false;
}

/**
 * @return true Potion is in bag
 * @return false No potions in bag
 */
bool Personnage::hasPotionInBag() const
{
    for (auto o : m_bag)
        if (o->getType() == 'b')
            return true;
    return false;
}

/**
 * @return bool: if the charactar is dead
**/
bool Personnage::isDead() const
{
    return m_health <= 0;
}

/**
 * @return : set m_health
**/
void Personnage::setHealth(double health)
{
    m_health = health;
}

/**
 * @return : set m_ability
**/
void Personnage::setAbility(double ability)
{
    m_ability = ability;
}

/**
 * @return vector<Objet *> m_bag
 */
vector<Objet *> Personnage::getBag() const
{
    return m_bag;
}

/**
 * destructor
**/
Personnage::~Personnage()
{
}
