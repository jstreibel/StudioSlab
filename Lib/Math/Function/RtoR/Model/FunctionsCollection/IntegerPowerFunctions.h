//
// Created by joao on 24/09/2019.
//

#ifndef V_SHAPE_INTEGERPOWERFUNCTIONS_H
#define V_SHAPE_INTEGERPOWERFUNCTIONS_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"


namespace Slab::Math::RtoR {
    class HarmonicDerivative : public RtoR::Function {
        Real m_sqr;
    public:
        explicit HarmonicDerivative(Real m_sqr) : m_sqr(m_sqr) { };
        Real operator()(Real x) const override { return m_sqr*x; }

        FunctionT<Real, Real> *Clone() const override { return new HarmonicDerivative(m_sqr); }

        Str symbol() const override {
            return "m²ϕ   m=" + ToStr(sqrt(m_sqr), 2);
        }

    };

    class HarmonicPotential : public RtoR::Function {
        Real m_sqr;
    public:
        explicit HarmonicPotential(Real massSqr=1.0) : m_sqr(massSqr) { };

        Real operator()(Real x) const override {
            return .5*m_sqr*x*x;
        }

        Ptr diff(int n) const override {
            return Ptr(new HarmonicDerivative(m_sqr));
        }

        FunctionT<Real, Real> *Clone() const override {
            return new HarmonicPotential();
        }

        Str symbol() const override {
            return "½m²ϕ²   m=" + ToStr(sqrt(m_sqr), 2);
        }
    };

    class Square : public RtoR::Function {
    public:
        Square() = default;

        Real operator()(Real x) const override {
            return x*x;
        }
    };

}


#endif //V_SHAPE_INTEGERPOWERFUNCTIONS_H
