//
// Created by joao on 27/05/24.
//

#include "LinearStepSolver.h"


namespace Slab::Math::Base {

    EquationState &LinearStepSolver::applyBC(EquationState &state, DevFloat t, DevFloat dt) {
        du->apply(state, t);
        return state;
    }

    EquationState &LinearStepSolver::operator()(const EquationState &in, EquationState &out, DevFloat t) {
        return this->F(in, out, t);
    }

    auto LinearStepSolver::NewEqState() const -> EquationState_ptr {
        return du->newEqState();
    }

}