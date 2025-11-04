//
// Created by joao on 11/05/24.
//

#ifndef STUDIOSLAB_RTORFUNCTIONSAMPLER_H
#define STUDIOSLAB_RTORFUNCTIONSAMPLER_H

#include "RtoRFunction.h"
#include "RtoRNumericFunction.h"

namespace Slab::Math::RtoR {
    class FunctionRenderer{
    public:
        static auto ToPointSet(const RtoR::Function &func, DevFloat xMin, DevFloat xMax, UInt resolution, DevFloat scale=1.0) -> Math::FPointSet_ptr;
        static auto ToNumericFunction(const RtoR::Function &func, RtoR::NumericFunction &output);
    };
}

#endif //STUDIOSLAB_RTORFUNCTIONSAMPLER_H
