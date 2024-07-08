//
// Created by joao on 7/6/24.
//

#include "AnalyticOscillon_1plus1d.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Oscillons/analytic-breather.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.h"

namespace Slab::Math::R2toR {

    AnalyticOscillon_1plus1d::AnalyticOscillon_1plus1d(AnalyticOscillon_1plus1d::OscillonParameters parameters)
    : oscParams(parameters) {   }

    Real AnalyticOscillon_1plus1d::operator()(Real2D x_mu) const {
        fix x = x_mu.x;
        fix t = x_mu.y;

        fix α = oscParams.alpha;

        fix l  = oscParams.l;
        fix v  = oscParams.v;
        fix u  = oscParams.u;

        fix x0 = oscParams.x0;
        fix t0 = oscParams.t0;

        RtoR::AnalyticOscillon oscillon(t-t0, x0, l, v, u, α, false, false);
        // oscillon.set_t(t);

        return oscillon(x);
    }

    void AnalyticOscillon_1plus1d::setParams(AnalyticOscillon_1plus1d::OscillonParameters parameters) {
        *this = AnalyticOscillon_1plus1d(parameters);
    }

    Pointer<Base::FunctionT<Real2D, Real>::Type> AnalyticOscillon_1plus1d::Clone() const {
        return New<AnalyticOscillon_1plus1d>(oscParams);
    }

} // Slab::Math::R2toR