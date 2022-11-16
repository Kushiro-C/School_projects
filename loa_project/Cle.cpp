#include "Cle.hpp"

/**
 * constructor
**/
Cle::Cle(const string name, const int x, const int y) :
Objet(name, 'k', "Une clé qui téléporte quelque part..."),  m_x(x), m_y(y)
{
}

/**
 * @return int get m_x
**/
int Cle::getX() const
{
    return m_x;
}

/**
 * @return int get m_y
**/
int Cle::getY() const
{
    return m_x;
}

Cle::~Cle()
{
}

/**
 * display key
**/
ostream& operator<<(ostream& out, Cle& x)
{
    x.printObjet(out);
    return out;
}

/**
 * Called from parent Objet
 */
void Cle::printObjet(ostream &where) const {
    where << "Objet : " << m_name << endl
        << "Type : " << m_type << endl
        << "Description : " << m_desc << endl
        << "Cle position (x:" << m_x << ", y:" << m_y << ")" << endl;
}