//
// Created by joao on 24/09/2019.
//

#ifndef V_SHAPE_INTEGERPOWERFUNCTIONS_H
#define V_SHAPE_INTEGERPOWERFUNCTIONS_H

#include "Lib/Fields/Maps/RtoR/Model/RtoRFunction.h"


namespace RtoR {
    class HarmonicDerivative : public Function {
    public:
        HarmonicDerivative() = default;
        Real operator()(Real x) const override { return x; }

        Function<Real, Real> *Clone() const override { return new HarmonicDerivative(); }
    };

    class HarmonicPotential : public Function {
    public:
        HarmonicPotential() = default;

        Real operator()(Real x) const override {
            return .5*x*x;
        }

        FunctionPtr diff(int n) const override {
            return FunctionPtr(new HarmonicDerivative());
        }

        Function<Real, Real> *Clone() const override {
            return new HarmonicPotential();
        }
    };

}


#endif //V_SHAPE_INTEGERPOWERFUNCTIONS_H
