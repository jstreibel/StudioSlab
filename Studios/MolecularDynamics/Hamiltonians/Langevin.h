#ifndef LANGEVIN_H
#define LANGEVIN_H

#include "NewtonMechanics.h"

#include "Utils/Types.h"
#include "Models/MolecularDynamics/MolDynNumericConfig.h"

namespace MolecularDynamics {
    using namespace Slab;

    class Langevin : public NewtonMechanics {
        Real T;

        auto F_nh(Real t) -> Graphics::Point2D override;
        static Graphics::Point2D xi();

    public:
        using Config = Slab::Models::MolecularDynamics::MolDynNumericConfig;
        Langevin(Pointer<Config> config, Real T);

        void setTemperature(Real T);
    };
}

#endif // LANGEVIN_H
