#include "Moine.hpp"

/**
 * constructor
**/
Moine::Moine(const string name) : Personnage(name, 25000, 115)
{
    vector<Objet *> starter = Objet::getStarterMoine();
    for (auto item : starter)
        addItemToBag(item); // +2000 HP
}

/**
 * special effect
 * @return a double, it could be used to change m_ability
 * represents character's special effect
**/
double Moine::special_effect(Personnage &target)
{
    double bonus_effect = 0.0;
    double enemyHpPercent = (target.m_health * 100) / target.m_maxHP; // XX %
    int chance = 30; // percentage XX %

    int rand_chance = rand()%2; // 0 or 1
    // If enemy HP is low, random chance to take more dmg
    if (rand_chance == 1 && enemyHpPercent <= 30 )
        bonus_effect += 0.15 * m_ability;

    int rand_crit = rand()%101; // 0 to 100
    if (rand_crit <= chance)
        bonus_effect += 0.05 * (m_health / 3);

    return bonus_effect;
}

/**
 * @brief Get the Class Name of Moine
 * @return string "moine"
 */
string Moine::getClassName()
{
    return "moine";
}

/**
 * operator print for Moine
**/
ostream& operator<<(ostream& out, Moine& x) {
    x.printCharacter(out);
    return out;
}

/**
 * Called from parent Personnage
 */
void Moine::printCharacter(ostream& where) const {
    where << "[Moine] " << m_name << endl;
    Personnage::printCharacter(where);
}

/**
 * destructor
**/
Moine::~Moine()
{
}

