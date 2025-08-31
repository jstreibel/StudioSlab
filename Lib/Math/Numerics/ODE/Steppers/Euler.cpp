//
// Created by joao on 26/03/25.
//

#include "Euler.h"

namespace Slab::Math {
    Euler::Euler(const TPointer<Base::LinearStepSolver> &solver, DevFloat dt)
    : _H(solver)
    , dt(dt)
    , _f(solver->NewEqState("Euler 'f'"))
    , _temp(solver->NewEqState("Euler 'temp'"))
    {

    }

    void Euler::Step(size_t n_steps) {

        auto &H = *_H;
        auto &f = *_f;
        auto &temp = *_temp;

        for (CountType i = 0; i < n_steps; ++i) {
            const DevFloat t = (step_counter + i) * dt;

            H.startStep(f, t, dt);
            H.applyBC(f, t, dt);

            f = f + (H(f, temp, t)*=dt);

            H.finishStep(f, t, dt);
        }
        step_counter += n_steps;
    }

    auto Euler::GetCurrentState() const -> TPointer<const Base::EquationState> {
        return _f;
    }
}