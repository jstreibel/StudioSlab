//
// Created by joao on 11/05/24.
//

#ifndef STUDIOSLAB_RTORFUNCTIONRENDERER_H
#define STUDIOSLAB_RTORFUNCTIONRENDERER_H

#include "RtoRFunction.h"
#include "RtoRDiscreteFunction.h"

namespace RtoR {
    class FunctionRenderer{
    public:
        static auto ToPointSet(const RtoR::Function &func, Real xMin, Real xMax, UInt resolution, Real scale=1.0) -> Math::PointSet::Ptr;
        static auto ToDiscreteFunction(const RtoR::Function &func, RtoR::DiscreteFunction &output);
    };
}

#endif //STUDIOSLAB_RTORFUNCTIONRENDERER_H
