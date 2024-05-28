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
#include "Math/Numerics/VoidBuilder.h"

#include <cstring> // contains memcpy
#include <omp.h>


namespace Slab::Math {

    class StepperRK4 : public Stepper {
    public:

        explicit StepperRK4(const Pointer<Base::Solver> &solver)
        : Stepper()
        , H(solver)
        , _phi    (solver->NewEqState())
        , _k1     (solver->NewEqState())
        , _k2     (solver->NewEqState())
        , _k3     (solver->NewEqState())
        , _k4     (solver->NewEqState())
        , _phiTemp(solver->NewEqState())
        {
            H->applyBC(*_phi, .0, .0);
        }

        ~StepperRK4() override = default;

        void step(const Real &dt, const size_t n_steps) override {
            const Real dt2 = 0.5 * dt;
            const Real dt6 = dt / 6.0;
            (void) dt6;
            const Real inv6 = 1.0 / 6.0;

            auto &phi = *_phi;
            auto &k1 = *_k1;
            auto &k2 = *_k2;
            auto &k3 = *_k3;
            auto &k4 = *_k4;
            auto &phiTemp = *_phiTemp;

            for (size_t i = 0; i < n_steps; ++i) {
                const Real t = (steps + i) * dt;

                {
                    H->startStep(phi, t, dt);
                    H->applyBC(phi, t, dt);

                    (*H)(phi, k1, t, dt2);

                    phiTemp.StoreAddition(phi, k1);

                    (*H)(phiTemp, k2, t, dt2);

                    phiTemp.StoreAddition(phi, k2);

                    (*H)(phiTemp, k3, t, dt);

                    phiTemp.StoreAddition(phi, k3);

                    (*H)(phiTemp, k4, t, dt);

                    ((k2 *= 2.0) += k3) *= 2.0;
                    (k1 *= 2.0) += k4;
                    phiTemp.StoreAddition(k1, k2) *= inv6;
                    phi += phiTemp;

                    H->finishStep(phi, t, dt);
                }
            }
            steps += n_steps;
        }

        [[nodiscard]] auto getCurrentState() const -> Base::EquationState_ptr override { return _phi; }

    private:
        Base::Solver_ptr H;

        Base::EquationState_ptr _phi;
        Base::EquationState_ptr _k1, _k2, _k3, _k4;
        Base::EquationState_ptr _phiTemp;

        int steps = 0;

    };


}


#endif /* STEPPER_NR_RK_H_ */
