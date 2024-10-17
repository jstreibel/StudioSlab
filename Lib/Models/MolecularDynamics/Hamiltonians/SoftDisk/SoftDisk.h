#ifndef SOFT_DISK_H
#define SOFT_DISK_H

#include "Models/MolecularDynamics/Hamiltonians/Langevin.h"
#include "Utils/Types.h"
#include "Models/MolecularDynamics/MolDynNumericConfig.h"

namespace MolecularDynamics {

    using namespace Slab;

    class SoftDisk : public Langevin {
    protected:
        Real U(const Graphics::Point2D &q1, const Graphics::Point2D &q2) override;

        Graphics::Point2D dUdr(const Graphics::Point2D &q1, const Graphics::Point2D &q2) override;

    public:
        using Config = Slab::Models::MolecularDynamics::MolDynNumericConfig;
        SoftDisk(Pointer<Config> config, Real T);

        static Real U(Real r);
    };

}

#endif // SOFT_DISK_H
