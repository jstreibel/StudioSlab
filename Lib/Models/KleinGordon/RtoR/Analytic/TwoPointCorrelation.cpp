//
// Created by joao on 10/6/24.
//

#include "TwoPointCorrelation.h"

namespace Slab::Models::KGRtoR {
    CorrelationDecay::CorrelationDecay(Real c0, Real ξ, Real β, Real u0)
    : c0(c0), invξ(1./ξ), β(β), u0(u0) {

    }

    Real CorrelationDecay::operator()(Real u) const {
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

    Real CorrelationDecay::get_c0() const {
        return c0;
    }

    void CorrelationDecay::set_c0(Real c_0) {
        CorrelationDecay::c0 = c_0;
    }

    Real CorrelationDecay::getξ() const {
        return 1./invξ;
    }

    void CorrelationDecay::setξ(Real ξ) {
        CorrelationDecay::invξ = 1. / ξ;
    }

    Real CorrelationDecay::getβ() const {
        return β;
    }

    void CorrelationDecay::setβ(Real beta) {
        CorrelationDecay::β = beta;
    }

    Real CorrelationDecay::getU0() const {
        return u0;
    }

    void CorrelationDecay::setU0(Real u0_new) {
        CorrelationDecay::u0 = u0_new;
    }


    TwoPointCorrelation::TwoPointCorrelation(Real c0, Real λ, Real ξ, Real β, Count n)
    : c(c0, ξ, β), λ(λ), n_max(n) {

    }

    Real TwoPointCorrelation::operator()(Real u) const {
        constexpr auto inv_picube = 32./(M_PI*M_PI*M_PI);
        Real sum = .0;

        int sign = 1;
        for(Count n=1; n<=n_max; n+=2) {
            fix inv_ncube = 1./Real(n*n*n);

            sum += sign*inv_ncube*cos(2*M_PI*Real(n)*u/λ);
            sign += -1;
        }


        return inv_picube * c(u) * sum;
    }

    Real TwoPointCorrelation::getλ() const { return λ; }
    void TwoPointCorrelation::setλ(Real lambda) { TwoPointCorrelation::λ = lambda; }

    Real TwoPointCorrelation::getξ() const { return c.getξ(); }
    void TwoPointCorrelation::setξ(Real ξ) { c.setξ(ξ); }

    Real TwoPointCorrelation::getβ() const { return c.getβ(); }
    void TwoPointCorrelation::setβ(Real beta) { c.setβ(beta); }

    Count TwoPointCorrelation::getNMax() const { return n_max; }
    void TwoPointCorrelation::setNMax(Count nMax) { n_max = nMax; }

    Real TwoPointCorrelation::get_c0() const { return c.get_c0(); }
    void TwoPointCorrelation::set_c0(Real c0) { c.set_c0(c0); }

    CorrelationDecay::Nature TwoPointCorrelation::getNature() const { return c.getNature(); }
    void TwoPointCorrelation::setNature(CorrelationDecay::Nature nature) { c.setNature(nature); }


} // Slab::Models::KGRtoR