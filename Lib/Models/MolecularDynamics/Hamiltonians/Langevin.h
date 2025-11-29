#ifndef LANGEVIN_H
#define LANGEVIN_H

#include "NewtonMechanics.h"

#include "Utils/Types.h"
#include "Models/MolecularDynamics/MolDynNumericConfig.h"

namespace Slab::Models::MolecularDynamics {
    using namespace Slab;

    class Langevin : public NewtonMechanics {
        DevFloat T;

        auto F_nh(DevFloat t) -> Graphics::FPoint2D override;
        static Graphics::FPoint2D xi();

    public:
        using Config = Slab::Models::MolecularDynamics::MolDynNumericConfig;
        Langevin(TPointer<Config> config, DevFloat T);

        void setTemperature(DevFloat T);
    };
}

#endif // LANGEVIN_H
