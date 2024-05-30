#ifndef STEPPER_H
#define STEPPER_H

#include "Utils/Types.h"
#include "Math/Numerics/Solver/EquationState.h"


namespace Slab::Math {

    class Stepper {
    public:
        Stepper() = default;

        virtual ~Stepper() = default;

    public:
        virtual void step(const Real &dt, size_t n_steps) = 0;

        virtual auto getCurrentState() const -> Base::EquationState_constptr = 0;
    };

    DefinePointer(Stepper)

}

#endif /* STEPPER_NR_RK_H_ */
