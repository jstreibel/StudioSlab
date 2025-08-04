//
// Created by joao on 27/05/24.
//

#include "LinearStepSolver.h"


namespace Slab::Math::Base {

    EquationState &LinearStepSolver::applyBC(EquationState &state, Real t, Real dt) {
        du->Apply(state, t);
        return state;
    }

    EquationState &LinearStepSolver::operator()(const EquationState &in, EquationState &out, Real t) {
        return this->F(in, out, t);
    }

    auto LinearStepSolver::NewEqState() const -> EquationState_ptr {
        return du->newEqState();
    }

}