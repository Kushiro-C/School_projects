#include "Sorciere.hpp"

/**
 * constructor
**/
Sorciere::Sorciere(const string name) : Personnage(name, 9000, 350)
{
    vector<Objet *> starter = Objet::getStarterSorciere();
    for (auto item : starter)
        addItemToBag(item); // +60 AP
}

/**
 * special effect
 * @return a double, it could be used to change m_ability
 * represents character's special effect
**/
double Sorciere::special_effect(Personnage &target)
{
    double bonus_effect = 0.0;
    double enemyHpPercent = (target.m_health * 100) / target.m_maxHP; // XX %
    double chance = 10; // percentage

    int rand_chance = rand()%2; // 0 or 1
    // If enemy HP is low, random chance to take more dmg
    if (rand_chance == 1 && enemyHpPercent <= 30 )
        bonus_effect += 0.15 * m_ability;

    int rand_crit = rand()%101; // 0 to 100
    if (rand_crit <= chance)
        bonus_effect += m_ability;

    return bonus_effect;
}

/**
 * @brief Get the Class Name of Sorciere
 * @return string "sorciere"
 */
string Sorciere::getClassName()
{
    return "sorciere";
}

/**
 * operator print for Sorciere
**/
ostream& operator<<(ostream& out, Sorciere& x) {
    x.printCharacter(out);
    return out;
}

/**
 * Called from parent Personnage
 */
void Sorciere::printCharacter(ostream& where) const {
    where << "[Sorcière] " << m_name << endl;
    Personnage::printCharacter(where);
}

/**
 * destructor
**/
Sorciere::~Sorciere()
{
}

