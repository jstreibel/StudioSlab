//
// Created by joao on 7/6/24.
//

#include "AnalyticOscillon_1plus1d.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Oscillons/analytic-breather.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.h"

namespace Slab::Math::R2toR {

    AnalyticOscillon_1plus1d::AnalyticOscillon_1plus1d(AnalyticOscillon_1plus1d::OscillonParameters parameters)
    : oscillon(0, parameters.x0, parameters.l, parameters.v, parameters.u, parameters.alpha, false, false)
    , oscParams(parameters) {   }

    Real AnalyticOscillon_1plus1d::operator()(Real2D x_mu) const {
        fix x = x_mu.x;
        fix t = x_mu.y;

        fix x0 = oscParams.x0;
        fix u = oscParams.u;
        fix ut = u*t;
        fix x0ut = x0+ut;
        fix λ = oscParams.l;
        fix γ⁻¹ = sqrt(1-u*u);

        if(Common::AreEqual(oscParams.v, 0.0, 1.e-6)) if(x < x0ut || x > (x0ut+λ*γ⁻¹)) return 0.0;

        const_cast<RtoR::AnalyticOscillon*>(&oscillon)->set_t(t);

        return oscillon(x);
    }

    void AnalyticOscillon_1plus1d::setParams(AnalyticOscillon_1plus1d::OscillonParameters parameters) {
        *this = AnalyticOscillon_1plus1d(parameters);
    }

    Pointer<Base::FunctionT<Real2D, Real>::Type> AnalyticOscillon_1plus1d::Clone() const {
        return New<AnalyticOscillon_1plus1d>(oscParams);
    }

} // Slab::Math::R2toR