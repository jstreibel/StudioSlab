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
        Langevin(const NumericConfig&, Real Temperature);

        void setTemperature(Real T);
    };
}

#endif // LANGEVIN_H
