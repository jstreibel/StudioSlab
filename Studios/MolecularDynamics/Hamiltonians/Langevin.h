#ifndef LANGEVIN_H
#define LANGEVIN_H

#include "NewtonMechanics.h"

#include "Common/Types.h"

namespace MolecularDynamics {
    class Langevin : public NewtonMechanics {
        Real T, dt;

        auto F_nh(Real t) -> Point2D override;
        static Point2D xi();

    public:
        Langevin(const NumericParams&, Real Temperature);
    };
}

#endif // LANGEVIN_H
