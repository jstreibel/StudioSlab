//
// Created by joao on 17/08/23.
//

#include "Section1D.h"
#include "Math/Function/RtoR/Model/RtoRFunctionSampler.h"
// #include "Graphics/FunctionRenderer.h"
#include <utility>

namespace Slab::Math {

    RtoR::Section1D::Section1D(TPointer<const R2toR::Function> function, TPointer<const RtoR2::ParametricCurve> curve)
            : function(std::move(function)), curve(std::move(curve)) {}

    DevFloat RtoR::Section1D::operator()(DevFloat x) const {
        if (curve == nullptr || function == nullptr) return NaN;

        const auto &c = *curve;
        const auto &func = *function;

        return func(c(x));
    }

    TPointer<Math::FPointSet> RtoR::Section1D::renderToPointSet(FunctionT<DevFloat, DevFloat>::RenderingOptions options) {
        if (curve == nullptr || function == nullptr) return {};

        return RtoR::FunctionRenderer::ToPointSet(*this, options.xMin, options.xMax, options.n);
    }


}