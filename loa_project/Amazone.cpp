#include "Amazone.hpp"

/**
 * constructor
**/
Amazone::Amazone(const string name) : Personnage(name, 9000, 400)
{
    vector<Objet *> starter = Objet::getStarterAmazone();
    for (auto item : starter)
        addItemToBag(item); // +70 AP
}

/**
 * special effect
 * @return a double, it could be used to change m_ability
 * represents character's special effect
**/
double Amazone::special_effect(Personnage &target)
{
    double bonus_effect = 0.0;
    double enemyHpPercent = (target.m_health * 100) / target.m_maxHP; // XX %
    int chance = 22; // percentage XX %

    int rand_chance = rand()%2; // 0 or 1
    // If enemy HP is low, random chance to take more dmg
    if (rand_chance == 1 && enemyHpPercent <= 30 )
        bonus_effect += 0.15 * m_ability;

    int rand_crit = rand()%101; // 0 to 100
    if (rand_crit <= chance)
        bonus_effect += 0.25 * m_ability;

    return bonus_effect;
}

/**
 * @brief Get the Class Name of Amazone
 * @return string "amazone"
 */
string Amazone::getClassName()
{
    return "amazone";
}

/**
 * operator print for Amazone
**/
ostream& operator<<(ostream& out, Amazone& x) {
    x.printCharacter(out);
    return out;
}

/**
 * Called from parent Personnage
 */
void Amazone::printCharacter(ostream& where) const {
    where << "[Amazone] " << m_name << endl;
    Personnage::printCharacter(where);
}

/**
 * destructor
**/
Amazone::~Amazone()
{
}

