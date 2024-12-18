//
// Created by joao on 10/28/24.
//

#ifndef STUDIOSLAB_MONTECARLOSTEPPER_H
#define STUDIOSLAB_MONTECARLOSTEPPER_H

#include "Math/Numerics/Stepper.h"
#include "MetropolisAlgorithm.h"

namespace Slab::Math {

    template<typename SiteType, typename ValueType>
    class MontecarloStepper : public Math::Stepper {
    public:
        using Algorithm = MetropolisAlgorithm<SiteType, ValueType>;
        using Setup = MetropolisSetup<SiteType, ValueType>;

        explicit MontecarloStepper(Pointer<Algorithm> algorithm) : algorithm(algorithm) { };
        MontecarloStepper() = delete;

        void step(size_t n_steps) override {
            for(auto i=0; i<n_steps; ++i) algorithm->step();
        }


        auto getCurrentState() const -> Pointer<const Base::EquationState> override {
            return Slab::Math::Base::EquationState_constptr();
        }

    private:

        Pointer<Algorithm> algorithm;
    };

} // Slab::Math

#endif //STUDIOSLAB_MONTECARLOSTEPPER_H
