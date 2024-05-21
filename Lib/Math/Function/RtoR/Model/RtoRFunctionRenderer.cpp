//
// Created by joao on 11/05/24.
//

#include "RtoRFunctionRenderer.h"


auto RtoR::FunctionRenderer::ToPointSet(const RtoR::Function &func, Real xMin, Real xMax, UInt resolution, Real scale) -> Math::PointSet_ptr {
    fix Δx = xMax-xMin;
    fix dx = Δx/(Real)resolution;

    std::vector<Real2D> pts(resolution);

    for(auto i=0; i<resolution; ++i){
        fix x = xMin + Real(i)*dx;
        pts[i] = {x, func(x)};
    }

    return Slab::New<Math::PointSet>(pts);
}


auto RtoR::FunctionRenderer::ToDiscreteFunction(const RtoR::Function &func, RtoR::DiscreteFunction &output) {
    output.Set(func);
}