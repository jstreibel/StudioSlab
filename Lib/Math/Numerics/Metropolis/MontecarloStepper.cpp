//
// Created by joao on 10/28/24.
//

#include "MontecarloStepper.h"

namespace Slab::Math {
    MontecarloStepper::MontecarloStepper(Pointer<MetropolisAlgorithm> algorithm)
    : algorithm(algorithm) { }

    void MontecarloStepper::step(size_t n_steps) {
        for(auto i=0; i<n_steps; ++i) algorithm->step();
    }

    Base::EquationState_constptr MontecarloStepper::getCurrentState() const {

        return Slab::Math::Base::EquationState_constptr();
    }


} // Slab::Math