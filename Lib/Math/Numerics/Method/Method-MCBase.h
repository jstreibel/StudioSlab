//
// Created by joao on 3/13/23.
//

#ifndef STUDIOSLAB_METHOD_EULER_H
#define STUDIOSLAB_METHOD_EULER_H

#endif //STUDIOSLAB_METHOD_EULER_H

#include "Stepper.h"

#include "Math/DifferentialEquations/BoundaryConditions.h"
#include "Math/DifferentialEquations/Solver.h"

#include <cstring> // contains memcpy
#include <omp.h>


namespace Slab::Math {

    template<int NUM_THREADS, class FIELD_STATE_TYPE>
    class StepperMontecarlo : public Stepper {
    public:
/*
        StepperMontecarlo(Base::NumericalRecipe &builder)
                : Stepper(), H(*(Base::EquationSolverT<FIELD_STATE_TYPE> *) builder.buildEquationSolver()),
                  dPhi(nullptr),
                  _phi(nullptr),
                  _phiTemp(nullptr)
                  /*dPhi((const Base::BoundaryConditions<FIELD_STATE_TYPE> *) builder.getBoundary()),
                  _phi((FIELD_STATE_TYPE *) builder.newFieldState()),
                  _phiTemp((FIELD_STATE_TYPE *) builder.newFieldState()) *//*
                   {
            // dPhi->apply(*_phi, 0.0);

            NOT_IMPLEMENTED
        }

        ~StepperMontecarlo() {
            delete &H;

            delete _phi;
            delete _phiTemp;
        }

        void step(const Real &dt, const size_t n_steps) override {

            auto &phi = *_phi;
            FIELD_STATE_TYPE *null = nullptr;

            for (size_t i = 0; i < n_steps; ++i) {
                const Real t = (steps + i) * dt;

                H.startStep(t, dt);
                {
                    H(*null, phi, t, dt);
#pragma omp barrier
                }
                H.finishStep(t, dt);
            }
            steps += n_steps;
        }

        [[nodiscard]] const void *getCurrentState() const override { return _phi; }

        [[nodiscard]] DiscreteSpacePair getSpaces() const override {
            return DiscreteSpacePair(&_phi->getPhi().getSpace(), &_phi->getDPhiDt().getSpace());
        }

    private:
        Base::EquationSolverT<FIELD_STATE_TYPE> &H;

        Pointer<const Base::BoundaryConditions<FIELD_STATE_TYPE>> dPhi;

        Pointer<FIELD_STATE_TYPE> _phi;
        Pointer<FIELD_STATE_TYPE>_phiTemp;

        int steps = 0;
*/

    };


}