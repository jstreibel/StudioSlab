//
// Created by joao on 24/09/2019.
//

#ifndef V_SHAPE_INTEGERPOWERFUNCTIONS_H
#define V_SHAPE_INTEGERPOWERFUNCTIONS_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"


namespace Slab::Math::RtoR {
    class FHarmonicDerivative : public RtoR::Function {
        DevFloat m_sqr;
    public:
        explicit FHarmonicDerivative(DevFloat m_sqr) : m_sqr(m_sqr) { };
        DevFloat operator()(DevFloat x) const override { return m_sqr*x; }

        Function_ptr Clone() const override { return New<FHarmonicDerivative>(m_sqr); }

        Str Symbol() const override {
            return "m²ϕ   m=" + ToStr(sqrt(m_sqr), 2);
        }

    };

    class FHarmonicPotential : public RtoR::Function {
        DevFloat m_sqr;
    public:
        explicit FHarmonicPotential(DevFloat massSqr=1.0) : m_sqr(massSqr) { };

        DevFloat operator()(DevFloat x) const override {
            return .5*m_sqr*x*x;
        }

        Function_ptr diff(int n) const override {
            return New<FHarmonicDerivative>(m_sqr);
        }

        Function_ptr Clone() const override {
            return New<FHarmonicPotential>();
        }

        Str Symbol() const override {
            return "½m²ϕ²   m=" + ToStr(sqrt(m_sqr), 2);
        }
    };

    class Square : public RtoR::Function {
    public:
        Square() = default;

        DevFloat operator()(DevFloat x) const override {
            return x*x;
        }
    };

    using HarmonicDerivative [[deprecated("Use FHarmonicDerivative")]] = FHarmonicDerivative;
    using HarmonicPotential [[deprecated("Use FHarmonicPotential")]] = FHarmonicPotential;

}


#endif //V_SHAPE_INTEGERPOWERFUNCTIONS_H
