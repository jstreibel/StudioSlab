//
// Created by joao on 26/03/25.
//

#include "Euler.h"

namespace Slab::Math {
    Euler::Euler(const Pointer<Base::LinearStepSolver> &solver, Real dt)
        : _H(solver), dt(dt)
    {
    }

    void Euler::step(size_t n_steps) {

        auto &H = *_H;
        auto &f = *_f;
        auto &temp = *_temp;

        for (Count i = 0; i < n_steps; ++i) {
            const Real t = (step_counter + i) * dt;

            H.startStep(f, t, dt);
            H.applyBC(f, t, dt);

            f = f + (H(f, temp, t)*=dt);

            H.finishStep(f, t, dt);
        }
        step_counter += n_steps;
    }

    auto Euler::getCurrentState() const -> Pointer<const Base::EquationState> {
        return _f;
    }
}