//
// Created by joao on 10/28/24.
//

#ifndef STUDIOSLAB_MONTECARLOSTEPPER_H
#define STUDIOSLAB_MONTECARLOSTEPPER_H

#include "Math/Numerics/Stepper.h"

namespace Slab::Math {

    class MontecarloStepper : public Math::Stepper {

    public:
        void step(size_t n_steps) override;

        auto getCurrentState() const -> Base::EquationState_constptr override;
    };

} // Slab::Math

#endif //STUDIOSLAB_MONTECARLOSTEPPER_H
