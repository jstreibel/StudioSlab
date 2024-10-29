//
// Created by joao on 10/28/24.
//

#ifndef STUDIOSLAB_R2TORMETROPOLISALGORITHM_H
#define STUDIOSLAB_R2TORMETROPOLISALGORITHM_H

#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Metropolis.h"
#include <functional>

namespace Slab::Math {

    struct RandomSite{ UInt i; UInt j; };

    using Temperature = Real;
    using NewValue = Real;

    using ΔEFunction          = std::function<Real(RandomSite, NewValue)>;
    using ModifyFunction      = std::function<void(RandomSite, NewValue)>;
    using SamplerFunction     = std::function<RandomSite()>;
    using ValueDrawerFunction = std::function<Real(RandomSite)>;

    struct MetropolisSetup {
        ΔEFunction ΔE;
        ModifyFunction modify;
        SamplerFunction sample;
        ValueDrawerFunction draw_value;
    };

    class MetropolisAlgorithmR2toR : public Metropolis {
        Pointer<R2toR::NumericFunction> function;
        MetropolisSetup algorithms;

    public:
        MetropolisAlgorithmR2toR(const Pointer<R2toR::NumericFunction_CPU>&, MetropolisSetup, Temperature);

        void step() override;
    };

} // Slab::Math

#endif //STUDIOSLAB_R2TORMETROPOLISALGORITHM_H
