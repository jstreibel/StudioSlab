//
// Created by joao on 10/6/24.
//

#include "TwoPointCorrelation.h"

namespace Slab::Models::KGRtoR {
    CorrelationDecay::CorrelationDecay(DevFloat c0, DevFloat ξ, DevFloat β, DevFloat u0)
    : c0(c0), invξ(1./ξ), β(β), u0(u0) {

    }

    DevFloat CorrelationDecay::operator()(DevFloat u) const {
        fix fuiξ = invξ * fabs(u-u0);

        if(nature==Exponential) {
            return c0 * exp(-β*fuiξ);
        }

        else if(nature==Power) {
            if (fuiξ > 1) return .0;

            return c0 * pow((1. - fuiξ), β);
        }

        NOT_IMPLEMENTED
    }

    CorrelationDecay::Nature CorrelationDecay::getNature() const { return nature; }
    void CorrelationDecay::setNature(CorrelationDecay::Nature nat) { nature = nat; }

    DevFloat CorrelationDecay::get_c0() const {
        return c0;
    }

    void CorrelationDecay::set_c0(DevFloat c_0) {
        CorrelationDecay::c0 = c_0;
    }

    DevFloat CorrelationDecay::getξ() const {
        return 1./invξ;
    }

    void CorrelationDecay::setξ(DevFloat ξ) {
        CorrelationDecay::invξ = 1. / ξ;
    }

    DevFloat CorrelationDecay::getβ() const {
        return β;
    }

    void CorrelationDecay::setβ(DevFloat beta) {
        CorrelationDecay::β = beta;
    }

    DevFloat CorrelationDecay::getU0() const {
        return u0;
    }

    void CorrelationDecay::setU0(DevFloat u0_new) {
        CorrelationDecay::u0 = u0_new;
    }


    TwoPointCorrelation::TwoPointCorrelation(DevFloat c0, DevFloat λ, DevFloat ξ, DevFloat β, CountType n)
    : c(c0, ξ, β), λ(λ), n_max(n) {

    }

    DevFloat TwoPointCorrelation::operator()(DevFloat u) const {
        constexpr auto inv_picube = 32./(M_PI*M_PI*M_PI);
        DevFloat sum = .0;

        int sign = 1;
        for(CountType n=1; n<=n_max; n+=2) {
            fix inv_ncube = 1./DevFloat(n*n*n);

            sum += sign*inv_ncube*cos(2*M_PI*DevFloat(n)*u/λ - ϕ_0);
            sign += -1;
        }


        return inv_picube * c(u) * sum;
    }

    DevFloat TwoPointCorrelation::getλ() const { return λ; }
    void TwoPointCorrelation::setλ(DevFloat lambda) { TwoPointCorrelation::λ = lambda; }

    DevFloat TwoPointCorrelation::getξ() const { return c.getξ(); }
    void TwoPointCorrelation::setξ(DevFloat ξ) { c.setξ(ξ); }

    DevFloat TwoPointCorrelation::getβ() const { return c.getβ(); }
    void TwoPointCorrelation::setβ(DevFloat beta) { c.setβ(beta); }

    CountType TwoPointCorrelation::getNMax() const { return n_max; }
    void TwoPointCorrelation::setNMax(CountType nMax) { n_max = nMax; }

    DevFloat TwoPointCorrelation::get_c0() const { return c.get_c0(); }
    void TwoPointCorrelation::set_c0(DevFloat c0) { c.set_c0(c0); }

    CorrelationDecay::Nature TwoPointCorrelation::getNature() const { return c.getNature(); }
    void TwoPointCorrelation::setNature(CorrelationDecay::Nature nature) { c.setNature(nature); }

    DevFloat TwoPointCorrelation::getΦ0() const { return ϕ_0; }

    void TwoPointCorrelation::setΦ0(DevFloat φ0) { ϕ_0 = φ0; }


} // Slab::Models::KGRtoR