//
// Created by joao on 9/08/23.
//

#include "Particle.h"

Molecule operator*(const Real &a, const Molecule &m){
    return {a*m.m_q, a*m.m_p};
}
Molecule operator+(const Molecule &a, const Molecule &b){
    return {a.m_q+b.m_q, a.m_p+b.m_p};
}
