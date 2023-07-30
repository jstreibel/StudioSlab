//
// Created by joao on 24/09/2019.
//

#ifndef V_SHAPE_INTEGERPOWERFUNCTIONS_H
#define V_SHAPE_INTEGERPOWERFUNCTIONS_H

#include "Mappings/RtoR/Model/RtoRFunction.h"


namespace RtoR {
    class HarmonicDerivative : public RtoR::Function {
    public:
        HarmonicDerivative() = default;
        Real operator()(Real x) const override { return x; }

        FunctionT<Real, Real> *Clone() const override { return new HarmonicDerivative(); }
    };

    class HarmonicPotential : public RtoR::Function {
    public:
        HarmonicPotential() = default;

        Real operator()(Real x) const override {
            return .5*x*x;
        }

        Ptr diff(int n) const override {
            return Ptr(new HarmonicDerivative());
        }

        FunctionT<Real, Real> *Clone() const override {
            return new HarmonicPotential();
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
