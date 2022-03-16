#ifndef LANGEVIN_H
#define LANGEVIN_H

#include "Hamiltonians/Types.h"
#include "SoftDiskParams.h"

class Langevin
{
public:
    Langevin(Real dt, Real T=Temperature);

    void operator() (const MoleculeContainer &m, MoleculeContainer &dmdt, const Real /* t */ );

private:
    Real U(const Point2D &q1, const Point2D &q2);
    Point2D mdUdr (const Point2D &q1, const Point2D &q2);
    Point2D xi();

private:
    Real T, dt;
};

#endif // LANGEVIN_H
