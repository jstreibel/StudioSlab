//
// Created by joao on 13/09/2019.
//

#ifndef V_SHAPE_PERTURBEDOSCILLONTIMEDERIVATIVE_H
#define V_SHAPE_PERTURBEDOSCILLONTIMEDERIVATIVE_H

#include "Lib/Fields/Maps/RtoR/Model/RtoRFunction.h"

namespace RtoR {
    class PerturbedOscillonTimeDerivative : public Function {
    public:
        PerturbedOscillonTimeDerivative(Real l, Real eps) : l(l), eps(eps) {}

        Real operator()(Real x) const override {
            if (x / l > -.5 && x / l <= 0.) return (x + .5 * l) * eps;
            else if (x / l > 0. && x / l <= .5) return (-x + .5 * l) * eps;

            return 0.0;
        }

    private:
        const Real l, eps;
    };
}


#endif //V_SHAPE_PERTURBEDOSCILLONTIMEDERIVATIVE_H
