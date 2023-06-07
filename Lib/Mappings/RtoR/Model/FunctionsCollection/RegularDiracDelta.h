//
// Created by joao on 13/09/2019.
//

#ifndef V_SHAPE_REGULARDIRACDELTA_H
#define V_SHAPE_REGULARDIRACDELTA_H

#include "Mappings/RtoR/Model/RtoRFunction.h"

namespace RtoR {
    class RegularDiracDelta : public Function {
    public:
        static auto deltaGauss(Real x, Real eps) -> Real {
            return 1. / sqrt(4 * M_PI * eps) * exp(-x * x / (4 * eps));
        }

        static auto deltaTri(Real x, Real eps) -> Real {
            //a = 2. * sqrt(M_PI * a); // mudanca do epsilon para ficar parecido com Gaussiana, conforme artigo

            const Real invEps = 1. / eps;

            if      (x > -eps && x <= .0)   return invEps * (1. + invEps * x);
            else if (x > .0   && x <   eps) return invEps * (1. - invEps * x);

            return 0.;
        }

        static auto deltaRect(Real x, Real eps) -> Real {
            if (x > -eps && x < eps) return 1. / (2. * eps);

            return 0.;
        }

        enum Regularization {
            Gaussian  = 0,
            Triangle  = 1,
            Rectangle = 2
        };

        RegularDiracDelta(Real eps, Real a, Regularization reg=Triangle, Real tx=0)
         : a(a), eps(eps), tx(tx), reg(reg) {
            switch (reg) {
                case Gaussian:
                    delta = RegularDiracDelta::deltaGauss;
                    break;
                case Triangle:
                    delta = RegularDiracDelta::deltaTri;
                    break;
                case Rectangle:
                    delta = RegularDiracDelta::deltaRect;
                    break;
            }
        }

        void setTranslation(Real _tx) { tx = _tx; }
        Real getTranslation() const { return tx; }

        auto operator()(Real x) const -> Real override {
            x-=tx;

            return a * delta(x, eps);
        }

        auto Clone() const -> Function<Real, Real> * override { return new RegularDiracDelta(eps, a, reg, tx); }

    private:
        Real (*delta)(Real, Real);

        const Real a, eps;
        Real tx;

        const Regularization reg;
    };
}

#endif //V_SHAPE_REGULARDIRACDELTA_H
