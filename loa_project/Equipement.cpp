#include "Equipement.hpp"

/**
 * constructor
**/
Equipement::Equipement(const string name, const int equipement_type,
                    const double puissance, const string desc)
    : Objet(name, 'e', desc),
    m_equipement_type(equipement_type), m_puissance(puissance)
{
}

/**
 * @return int 0 for waepon, 1 for shield
**/
int Equipement::getEquipement_type() const
{
    return m_equipement_type;
}

/**
 * @return double m_puissance
**/
double Equipement::getPuissance() const
{
    return m_puissance;
}

/**
 * use the shield/waepon on target
**/
void Equipement::equip(Personnage &target)
{
    if (m_equipement_type == 0) { // Shield/Armor -> + HP
        if (target.m_health == target.m_maxHP) // full HP before equipping
            target.setHealth(target.m_health + m_puissance);
        target.m_maxHP += m_puissance;
    } else // Weapon -> + AP
        target.setAbility(target.m_ability + m_puissance);
}


/**
 * unequip the shield/waepon on target
**/
void Equipement::unEquip(Personnage &target)
{
    if (m_equipement_type == 0) { // Shield/Armor -> + HP
        target.m_maxHP -= m_puissance;
        if (target.m_health > target.m_maxHP) // Adjust HP
            target.setHealth(target.m_maxHP);
    } else // Weapon -> - AP
        target.setAbility(target.m_ability - m_puissance);
}

Equipement::~Equipement()
{
}

/**
 * display equipment
**/
ostream& operator<<(ostream& out, Equipement& x)
{
    x.printObjet(out);
    return out;
}

/**
 * Called from parent Objet
 */
void Equipement::printObjet(ostream &where) const {
    where << "Objet : " << m_name << endl
        << "Type : " << m_type << endl;
    if (m_equipement_type == 0) // shield
        where << "Bouclier de puissance " << m_puissance << endl;
    else // 1 : weapon
        where << "Arme de puissance " << m_puissance << endl;
    where << "Description : " << m_desc << endl;
}