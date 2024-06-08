#ifndef SOFT_DISK_H
#define SOFT_DISK_H

#include "Hamiltonians/Langevin.h"
#include "Utils/Types.h"

namespace MolecularDynamics {

    using namespace Slab;

    class SoftDisk : public Langevin {
    protected:
        Real U(const Graphics::Point2D &q1, const Graphics::Point2D &q2) override;

        Graphics::Point2D dUdr(const Graphics::Point2D &q1, const Graphics::Point2D &q2) override;

    public:
        SoftDisk(const Math::NumericConfig &p, Real T);

        static Real U(Real r);
    };

}

#endif // SOFT_DISK_H
