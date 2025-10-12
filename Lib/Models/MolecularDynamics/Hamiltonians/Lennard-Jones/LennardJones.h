#ifndef LENNARD_JONES_H
#define LENNARD_JONES_H

#include "Models/MolecularDynamics/Hamiltonians/Langevin.h"

namespace Slab::Models::MolecularDynamics {

    using Config = Slab::Models::MolecularDynamics::MolDynNumericConfig;

    class LennardJones final : public Langevin {
    protected:
        DevFloat U(const Graphics::Point2D &q1, const Graphics::Point2D &q2) override;

        Graphics::Point2D dUdr(const Graphics::Point2D &q1, const Graphics::Point2D &q2) override;

    public:
        LennardJones(const TPointer<Config>& config, DevFloat T);

        static DevFloat U(DevFloat r);
    };

}



#endif // LENNARD_JONES_H
