//
// Created by joao on 11/05/24.
//

#include "RtoRFunctionSampler.h"


namespace Slab::Math {

    auto RtoR::FunctionRenderer::ToPointSet(const RtoR::Function &func, DevFloat xMin, DevFloat xMax, UInt resolution,
                                            DevFloat scale) -> Math::PointSet_ptr {
        fix Δx = xMax - xMin;
        fix dx = Δx / (DevFloat) resolution;

        Vector<Real2D> pts(resolution);

        for (auto i = 0; i < resolution; ++i) {
            fix x = xMin + DevFloat(i) * dx;
            pts[i] = {x, func(x)};
        }

        return Slab::New<Math::PointSet>(pts);
    }


    auto RtoR::FunctionRenderer::ToNumericFunction(const RtoR::Function &func, RtoR::NumericFunction &output) {
        output.Set(func);
    }

}