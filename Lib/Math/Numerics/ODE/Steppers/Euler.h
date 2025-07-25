//
// Created by joao on 26/03/25.
//

#ifndef EULER_H
#define EULER_H
#include <Math/Numerics/Stepper.h>
#include <Math/Numerics/ODE/Solver/LinearStepSolver.h>

namespace Slab::Math {

    class Euler final : public Stepper {
        TPointer<Base::LinearStepSolver> _H;

        Base::EquationState_ptr _f;
        Base::EquationState_ptr _temp;

        CountType step_counter = 0;

        DevFloat dt;

    public:
        explicit Euler(const TPointer<Base::LinearStepSolver> &solver, DevFloat dt);

        void step(size_t n_steps) override;

        auto getCurrentState() const -> TPointer<const Base::EquationState> override;
    };

}
#endif //EULER_H
