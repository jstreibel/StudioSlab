//
// Created by joao on 10/28/24.
//

#ifndef STUDIOSLAB_MONTECARLOSTEPPER_H
#define STUDIOSLAB_MONTECARLOSTEPPER_H

#include "Math/Numerics/Stepper.h"
#include "MetropolisAlgorithm.h"

namespace Slab::Math {

    class MontecarloStepper : public Math::Stepper {
        Pointer<MetropolisAlgorithm> algorithm;
    public:
        explicit MontecarloStepper(Pointer<MetropolisAlgorithm> algorithm);
        MontecarloStepper() = delete;

        void step(size_t n_steps) override;

        auto getCurrentState() const -> Pointer<const Base::EquationState> override;
    };

} // Slab::Math

#endif //STUDIOSLAB_MONTECARLOSTEPPER_H
