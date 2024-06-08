#ifndef LANGEVIN_H
#define LANGEVIN_H

#include "NewtonMechanics.h"

#include "Utils/Types.h"

namespace MolecularDynamics {
    using namespace Slab;

    class Langevin : public NewtonMechanics {
        Real T, dt;

        auto F_nh(Real t) -> Graphics::Point2D override;
        static Graphics::Point2D xi();

    public:
        Langevin(const Math::NumericConfig&, Real Temperature);

        void setTemperature(Real T);
    };
}

#endif // LANGEVIN_H
