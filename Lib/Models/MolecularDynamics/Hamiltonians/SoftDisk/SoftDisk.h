#ifndef SOFT_DISK_H
#define SOFT_DISK_H

#include "Models/MolecularDynamics/Hamiltonians/Langevin.h"
#include "Utils/Types.h"
#include "Models/MolecularDynamics/MolDynNumericConfig.h"

namespace Slab::Models::MolecularDynamics {

    using namespace Slab;

    class SoftDisk : public Langevin {
    protected:
        DevFloat U(const Graphics::Point2D &q1, const Graphics::Point2D &q2) override;

        Graphics::Point2D dUdr(const Graphics::Point2D &q1, const Graphics::Point2D &q2) override;

    public:
        using Config = Slab::Models::MolecularDynamics::MolDynNumericConfig;
        SoftDisk(Pointer<Config> config, DevFloat T);

        static DevFloat U(DevFloat r);
    };

}

#endif // SOFT_DISK_H
