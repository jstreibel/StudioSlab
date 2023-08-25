//
// Created by joao on 17/08/23.
//

#include "Section1D.h"
#include "Mappings/FunctionRenderer.h"
#include <utility>

RtoR::Section1D::Section1D(R2toR::Function::ConstPtr function, RtoR2::ParametricCurve::ConstPtr curve)
: function(std::move(function)), curve(std::move(curve))
{   }

RtoR::Section1D::Section1D(const R2toR::Function *function, const RtoR2::ParametricCurve *curve)
: Section1D(DummyPtr(*function), DummyPtr(*curve))
{   }

Real RtoR::Section1D::operator()(Real x) const {
    const auto &c = *curve;
    const auto &func = *function;

    return func(c(x));
}

Spaces::PointSet::Ptr RtoR::Section1D::renderToPointSet(Base::FunctionT<Real, Real>::RenderingOptions options) {
    return RtoR::FunctionRenderer::toPointSet(*this, options.xMin, options.xMax, options.n);
}


