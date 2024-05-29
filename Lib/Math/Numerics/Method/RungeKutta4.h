/*
 * stepper-rk.h
 *
 *  Created on: 16 de set de 2017
 *      Author: johnny
 */

#ifndef STEPPER_RK_H
#define STEPPER_RK_H

#include "Stepper.h"

#include "Math/DifferentialEquations/BoundaryConditions.h"
#include "Math/DifferentialEquations/Solver.h"
#include "Math/Numerics/NumericalRecipe.h"

#include <cstring> // contains memcpy
#include <omp.h>


namespace Slab::Math {

    class RungeKutta4 : public Stepper {
    public:

        explicit RungeKutta4(const Pointer<Base::Solver> &solver)
        : Stepper()
        , _H(solver)
        , _f    (solver->NewEqState())
        , _k1     (solver->NewEqState())
        , _k2     (solver->NewEqState())
        , _k3     (solver->NewEqState())
        , _k4     (solver->NewEqState())
        , _temp(solver->NewEqState())
        {
            _H->applyBC(*_f, .0, .0);
        }

        ~RungeKutta4() override = default;

        void step(const Real &dt, const Count n_steps) override {
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

                    H(f, k1, t, dt/2);

                    temp = f + k1;

                    H(temp, k2, t, dt/2);

                    temp = f + k2;

                    H(temp, k3, t, dt);

                    temp = f + k3;

                    H(temp, k4, t, dt);

                    ((k2 *= 2.0) += k3) *= 2.0;
                    (k1 *= 2.0) += k4;
                    (temp = k1 + k2) *= 1./6;
                    f += temp;

                    H.finishStep(f, t, dt);
                }
            }
            steps += n_steps;
        }

        [[nodiscard]] auto getCurrentState() const -> Base::EquationState_ptr override { return _f; }

    private:
        Base::Solver_ptr _H;

        Base::EquationState_ptr _f, _k1, _k2, _k3, _k4;
        Base::EquationState_ptr _temp;

        Count steps = 0;

    };


}


#endif /* STEPPER_NR_RK_H_ */
