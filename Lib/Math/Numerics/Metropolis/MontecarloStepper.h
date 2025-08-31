//
// Created by joao on 10/28/24.
//

#ifndef STUDIOSLAB_MONTECARLOSTEPPER_H
#define STUDIOSLAB_MONTECARLOSTEPPER_H

#include "Math/Numerics/Stepper.h"
#include "MetropolisAlgorithm.h"

namespace Slab::Math {

    template<typename SiteType, typename ValueType>
    class MontecarloStepper : public Math::FStepper {
    public:
        using Algorithm = MetropolisAlgorithm<SiteType, ValueType>;
        using Setup = MetropolisSetup<SiteType, ValueType>;

        explicit MontecarloStepper(TPointer<Algorithm> algorithm) : algorithm(algorithm) { };
        MontecarloStepper() = delete;

        void Step(size_t n_steps) override {
            for(auto i=0; i<n_steps; ++i) algorithm->step();
        }


        auto GetCurrentState() const -> TPointer<const Base::EquationState> override {
            return Slab::Math::Base::EquationState_constptr();
        }

    private:

        TPointer<Algorithm> algorithm;
    };

} // Slab::Math

#endif //STUDIOSLAB_MONTECARLOSTEPPER_H
