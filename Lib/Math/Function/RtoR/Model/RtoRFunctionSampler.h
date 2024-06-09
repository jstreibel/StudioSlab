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
        static auto ToPointSet(const RtoR::Function &func, Real xMin, Real xMax, UInt resolution, Real scale=1.0) -> Math::PointSet_ptr;
        static auto ToNumericFunction(const RtoR::Function &func, RtoR::NumericFunction &output);
    };
}

#endif //STUDIOSLAB_RTORFUNCTIONSAMPLER_H
