#ifndef LANGEVIN_H
#define LANGEVIN_H

#include "SoftDiskParams.h"
#include "../NewtonMechanics.h"
#include "../../Particle.h"

#include "Common/Types.h"

namespace MolecularDynamics {
    class Langevin : public NewtonMechanics {
    public:
        Langevin(const NumericParams&, Real T = Temperature);

        void operator()(const MoleculeContainer &m, MoleculeContainer &dmdt, const Real /* t */);

    private:
        Real U(const Point2D &q1, const Point2D &q2);

        Point2D mdUdr(const Point2D &q1, const Point2D &q2);

        Point2D xi();

    private:
        Real T, dt;
    };
}

#endif // LANGEVIN_H
