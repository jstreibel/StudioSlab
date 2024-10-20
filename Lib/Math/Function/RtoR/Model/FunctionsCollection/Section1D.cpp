//
// Created by joao on 17/08/23.
//

#include "Section1D.h"
#include "Math/Function/RtoR/Model/RtoRFunctionSampler.h"
// #include "Graphics/FunctionRenderer.h"
#include <utility>

namespace Slab::Math {

    RtoR::Section1D::Section1D(R2toR::Function_constptr function, RtoR2::ParametricCurve_constptr curve)
            : function(std::move(function)), curve(std::move(curve)) {}

    Real RtoR::Section1D::operator()(Real x) const {
        if (curve == nullptr || function == nullptr) return NaN;

        const auto &c = *curve;
        const auto &func = *function;

        return func(c(x));
    }

    Math::PointSet_ptr RtoR::Section1D::renderToPointSet(FunctionT<Real, Real>::RenderingOptions options) {
        if (curve == nullptr || function == nullptr) return {};

        return RtoR::FunctionRenderer::ToPointSet(*this, options.xMin, options.xMax, options.n);
    }


}