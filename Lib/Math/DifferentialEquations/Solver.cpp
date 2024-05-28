//
// Created by joao on 27/05/24.
//

#include "Solver.h"


namespace Slab::Math::Base {

    EquationState &Solver::applyBC(EquationState &state, Real t, Real dt) {
        du->apply(state, t);
        return state;
    }

    EquationState &Solver::operator()(const EquationState &in, EquationState &out, Real t, Real dt) {
        return this->dtF(in, out, t, dt);
    }

    auto Solver::NewEqState() const -> EquationState_ptr {
        return du->newEqState();
    }

}