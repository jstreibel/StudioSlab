//
// Created by joao on 9/08/23.
//

#include "Particle.h"

namespace Slab::Models::MolecularDynamics {

    Molecule operator*(const DevFloat &a, const Molecule &m) {
        return {a * m.m_q, a * m.m_p};
    }

    Molecule operator+(const Molecule &a, const Molecule &b) {
        return {a.m_q + b.m_q, a.m_p + b.m_p};
    }

}