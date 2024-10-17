//
// Created by joao on 5/29/24.
//

#include "RungeKutta4.h"


namespace Slab::Math {
    RungeKutta4::RungeKutta4(const Pointer<Base::Solver> &solver, Real dt, Slab::Count totalSwapStates)
    : Stepper()
    , _H(solver)
    , _f(solver->NewEqState())
    , _k1(solver->NewEqState())
    , _k2(solver->NewEqState())
    , _k3(solver->NewEqState())
    , _k4(solver->NewEqState())
    , _temp(solver->NewEqState())
    , swapsIterator(swaps)
    , dt(dt) {
        for (auto i = 0; i < totalSwapStates; ++i)
            swaps.push_back(solver->NewEqState());

        _H->applyBC(*_f, .0, .0);
    }

    void RungeKutta4::step(const Count n_steps) {
        auto swap = swapsIterator.next();
        swap->setData(*_f);

        auto &H = *_H;
        auto &f = *_f;
        auto &k1 = *_k1;
        auto &k2 = *_k2;
        auto &k3 = *_k3;
        auto &k4 = *_k4;
        auto &temp = *_temp;

        for (Count i = 0; i < n_steps; ++i) {
            const Real t = (steps + i) * dt;

            {
                H.startStep(f, t, dt);
                H.applyBC(f, t, dt);

                temp = f + (H(f, k1, t)*=dt/2);
                temp = f + (H(temp, k2, t)*=dt/2);
                temp = f + (H(temp, k3, t)*=dt);

                H(temp, k4, t) *= dt;

                ((k2 *= 2.0) += k3) *= 2.0;
                (k1 *= 2.0) += k4;
                temp = k1 + k2;
                f += (temp*=(1./6));

                H.finishStep(f, t, dt);
            }
        }
        steps += n_steps;
    }

    auto RungeKutta4::getCurrentState() const -> Base::EquationState_constptr { return _f; }

}