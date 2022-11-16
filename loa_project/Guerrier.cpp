#include "Guerrier.hpp"

/**
 * constructor
**/
Guerrier::Guerrier(const string name) : Personnage(name, 19000, 140)
{
    vector<Objet *> starter = Objet::getStarterGuerrier();
    for (auto item : starter)
        addItemToBag(item); // +50 AP
}

/**
 * special effect
 * @return a double, it could be used to change m_ability
 * represents character's special effect
**/
double Guerrier::special_effect(Personnage &target)
{
    double bonus_effect = 0.0;
    double enemyHpPercent = (target.m_health * 100) / target.m_maxHP; // XX %
    int chance = 15; // percentage XX %

    int rand_chance = rand()%2; // 0 or 1
    // If enemy HP is low, random chance to take more dmg
    if (rand_chance == 1 && enemyHpPercent <= 30 )
        bonus_effect += 0.15 * m_ability;

    int rand_crit = rand()%101; // 0 to 100
    if (rand_crit <= chance)
        bonus_effect += 0.15 * m_ability;

    return bonus_effect;
}

/**
 * @brief Get the Class Name of Guerrier
 * @return string "guerrier"
 */
string Guerrier::getClassName()
{
    return "guerrier";
}

/**
 * operator print for Guerrier
**/
ostream& operator<<(ostream& out, Guerrier& x) {
    x.printCharacter(out);
    return out;
}

/**
 * Called from parent Personnage
 */
void Guerrier::printCharacter(ostream& where) const {
    where << "[Guerrier] " << m_name << endl;
    Personnage::printCharacter(where);
}

/**
 * destructor
**/
Guerrier::~Guerrier()
{
}

