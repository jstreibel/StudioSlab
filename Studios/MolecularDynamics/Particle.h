//
// Created by joao on 9/08/23.
//

#ifndef STUDIOSLAB_PARTICLE_H
#define STUDIOSLAB_PARTICLE_H

#include "Common/Types.h"

#define CUTOFF_RADIUS 4

// Used for Verlet
typedef std::pair<PointContainer &, PointContainer &> State;
typedef std::pair<PointContainer, PointContainer> VerletMoleculeContainer;

// Used for RK4
struct Molecule {
    Molecule(Point2D q, Point2D p) : m_q(q), m_p(p) {}
    Molecule() = default;

    Point2D m_q, m_p;
};

Molecule operator*(const Real &a, const Molecule &m);
Molecule operator+(const Molecule &a, const Molecule &b);

typedef std::vector<Molecule> MoleculeContainer;


#endif //STUDIOSLAB_PARTICLE_H
