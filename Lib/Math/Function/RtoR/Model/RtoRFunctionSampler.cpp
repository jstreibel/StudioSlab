//
// Created by joao on 11/05/24.
//

#include "RtoRFunctionSampler.h"


namespace Slab::Math {

    auto RtoR::FunctionRenderer::ToPointSet(const RtoR::Function &func, Real xMin, Real xMax, UInt resolution,
                                            Real scale) -> Math::PointSet_ptr {
        fix Δx = xMax - xMin;
        fix dx = Δx / (Real) resolution;

        Vector<Real2D> pts(resolution);

        for (auto i = 0; i < resolution; ++i) {
            fix x = xMin + Real(i) * dx;
            pts[i] = {x, func(x)};
        }

        return Slab::New<Math::PointSet>(pts);
    }


    auto RtoR::FunctionRenderer::ToNumericFunction(const RtoR::Function &func, RtoR::NumericFunction &output) {
        output.Set(func);
    }

}