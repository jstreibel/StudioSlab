#ifndef STEPPER_H
#define STEPPER_H

#include "Utils/Types.h"
#include "Math/Numerics/ODE/Solver/EquationState.h"


namespace Slab::Math {

    class FStepper {
    public:
        FStepper() = default;

        virtual ~FStepper() = default;

    public:
        virtual void Step(size_t n_steps) = 0;

        virtual auto GetCurrentState() const -> Base::EquationState_constptr = 0;
    };

    DefinePointers(FStepper)

}

#endif /* STEPPER_NR_RK_H_ */
