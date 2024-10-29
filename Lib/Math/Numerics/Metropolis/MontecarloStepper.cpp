//
// Created by joao on 10/28/24.
//

#include "MontecarloStepper.h"

namespace Slab::Math {
    void MontecarloStepper::step(size_t n_steps) {

    }

    Base::EquationState_constptr MontecarloStepper::getCurrentState() const {
        return Slab::Math::Base::EquationState_constptr();
    }
} // Slab::Math