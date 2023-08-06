
#include "Types.h"

#include <assert.h>


Point2D operator + (const Point2D &a, const Point2D &b){ return {a.x+b.x, a.y+b.y}; }
Point2D operator - (const Point2D &a, const Point2D &b){ return {a.x-b.x, a.y-b.y}; }
Point2D operator * (const Real &a, const Point2D &p){ return {p.x*a, p.y*a}; }


Molecule operator*(const Real &a, const Molecule &m){
    return {a*m.m_q, a*m.m_p};
}
Molecule operator+(const Molecule &a, const Molecule &b){
    return {a.m_q+b.m_q, a.m_p+b.m_p};
}

