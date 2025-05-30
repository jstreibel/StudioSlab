//
// Created by joao on 26/03/25.
//

#ifndef EULER_H
#define EULER_H
#include <Math/Numerics/Stepper.h>
#include <Math/Numerics/ODE/Solver/LinearStepSolver.h>

namespace Slab::Math {

    class Euler final : public Stepper {
        Pointer<Base::LinearStepSolver> _H;

        Base::EquationState_ptr _f;
        Base::EquationState_ptr _temp;

        Count step_counter = 0;

        Real dt;

    public:
        explicit Euler(const Pointer<Base::LinearStepSolver> &solver, Real dt);

        void step(size_t n_steps) override;

        auto getCurrentState() const -> Pointer<const Base::EquationState> override;
    };

}
#endif //EULER_H
