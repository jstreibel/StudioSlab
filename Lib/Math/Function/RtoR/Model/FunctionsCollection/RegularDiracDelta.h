//
// Created by joao on 13/09/2019.
//

#ifndef V_SHAPE_REGULARDIRACDELTA_H
#define V_SHAPE_REGULARDIRACDELTA_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

#include "Math/Toolset/NativeFunctions.h"

namespace Slab::Math::RtoR {
    class RegularDiracDelta : public RtoR::Function {
        DevFloat (*delta)(DevFloat, DevFloat);

        const DevFloat a, eps;
        DevFloat tx;

    public:
        enum Regularization {
            Gaussian  = 0,
            Triangle  = 1,
            Rectangle = 2
        };

    private:
        const Regularization reg;

    public:

        RegularDiracDelta(DevFloat eps, DevFloat a, Regularization reg=Triangle, DevFloat tx=0)
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

        void setTranslation(DevFloat _tx) { tx = _tx; }
        DevFloat getTranslation() const { return tx; }

        auto operator()(DevFloat x) const -> DevFloat override {
            x-=tx;

            return a * delta(x, eps);
        }

        auto Clone() const -> RtoR::Function_ptr override { return New <RegularDiracDelta> (eps, a, reg, tx); }
    };
}

#endif //V_SHAPE_REGULARDIRACDELTA_H
