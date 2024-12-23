﻿#ifndef LENNARD_JONES_H
#define LENNARD_JONES_H

#include "Models/MolecularDynamics/Hamiltonians/Langevin.h"

namespace Slab::Models::MolecularDynamics {

    using Config = Slab::Models::MolecularDynamics::MolDynNumericConfig;

    class LennardJones : public Langevin {
    protected:
        Real U(const Graphics::Point2D &q1, const Graphics::Point2D &q2) override;

        Graphics::Point2D dUdr(const Graphics::Point2D &q1, const Graphics::Point2D &q2) override;

    public:
        LennardJones(Pointer<Config> config, Real T);

        static Real U(Real r);
    };

}



#endif // LENNARD_JONES_H
