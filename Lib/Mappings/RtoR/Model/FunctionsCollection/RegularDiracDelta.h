//
// Created by joao on 13/09/2019.
//

#ifndef V_SHAPE_REGULARDIRACDELTA_H
#define V_SHAPE_REGULARDIRACDELTA_H

#include "Mappings/RtoR/Model/RtoRFunction.h"

#include "Phys/Toolset/NativeFunctions.h"

namespace RtoR {
    class RegularDiracDelta : public FunctionT {
    public:

        enum Regularization {
            Gaussian  = 0,
            Triangle  = 1,
            Rectangle = 2
        };

        RegularDiracDelta(Real eps, Real a, Regularization reg=Triangle, Real tx=0)
         : a(a), eps(eps), tx(tx), reg(reg) {
            switch (reg) {
                case Gaussian:
                    delta = deltaGauss;
                    break;
                case Triangle:
                    delta = deltaTri;
                    break;
                case Rectangle:
                    delta = deltaRect;
                    break;
            }
        }

        void setTranslation(Real _tx) { tx = _tx; }
        Real getTranslation() const { return tx; }

        auto operator()(Real x) const -> Real override {
            x-=tx;

            return a * delta(x, eps);
        }

        auto Clone() const -> FunctionT<Real, Real> * override { return new RegularDiracDelta(eps, a, reg, tx); }

    private:
        Real (*delta)(Real, Real);

        const Real a, eps;
        Real tx;

        const Regularization reg;
    };
}

#endif //V_SHAPE_REGULARDIRACDELTA_H
