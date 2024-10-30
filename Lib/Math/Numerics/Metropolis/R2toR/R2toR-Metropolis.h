//
// Created by joao on 10/28/24.
//

#ifndef STUDIOSLAB_R2TOR_METROPOLIS_H
#define STUDIOSLAB_R2TOR_METROPOLIS_H

#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Numerics/Metropolis/MetropolisAlgorithm.h"
#include "R2toR-Metropolis-Setup.h"

namespace Slab::Math {

    class R2toRMetropolis : public MetropolisAlgorithm {
        Pointer<R2toR::NumericFunction> function;
        R2toRMetropolisSetup algorithms;

    public:
        R2toRMetropolis(const Pointer<R2toR::NumericFunction_CPU>&, R2toRMetropolisSetup, Temperature);

        void step() override;
    };

} // Slab::Math

#endif //STUDIOSLAB_R2TOR_METROPOLIS_H
