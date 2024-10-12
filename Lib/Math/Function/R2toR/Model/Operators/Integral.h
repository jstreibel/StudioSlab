//
// Created by joao on 10/11/24.
//

#ifndef STUDIOSLAB_INTEGRAL_H
#define STUDIOSLAB_INTEGRAL_H

#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"
#include "Math/FunctionOperations/FunctionOperation.h"

namespace Slab::Math::R2toR {

    class Integral : public Base::FunctionOperation<R2toR::NumericFunction_CPU, RtoR::NumericFunction_CPU> {
    public:
        struct Config {
            enum Direction { dx, dy } direction = dx;
            bool compute_avg=false;
        } config;

        explicit Integral(Config config);

        auto operator[](Pointer<NumericFunction_CPU>) -> Pointer<RtoR::NumericFunction_CPU> override;
    };

} // Slab::Math::R2toR

#endif //STUDIOSLAB_INTEGRAL_H
