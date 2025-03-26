//
// Created by joao on 25/03/25.
//

#ifndef SPIBC_H
#define SPIBC_H
#include <Math/Numerics/ODE/Solver/BoundaryConditions.h>

namespace Slab::Models::StochasticPathIntegrals {

    class SPIBC : public Math::Base::BoundaryConditions {
    public:
        explicit SPIBC(const Pointer<const Math::Base::EquationState> &prototype)
            : BoundaryConditions(prototype) {
        }

        void apply(Math::Base::EquationState &toFunction, Real t) const override;
    };

}

#endif //SPIBC_H
