//
// Created by joao on 08/06/23.
//

#include "RingDeltaFunc.h"
#include "Mappings/R2toR/Model/R2toRDiscreteFunctionGPU.h"

namespace R2toR {
    namespace LeadingDelta {


        RingDeltaFunc::RingDeltaFunc(Real eps, Real a, Real dt) : eps(eps), a(a), dt(dt), radius(0) {
        }

        auto RingDeltaFunc::getEps()                      const -> Real   { return eps;       }
        auto RingDeltaFunc::getA()                        const -> Real   { return a;         }

        auto RingDeltaFunc::setRadius(Real _radius)             -> void   { radius = _radius; }
        auto RingDeltaFunc::getRadius()                   const -> Real   { return radius;    }

        auto RingDeltaFunc::domainContainsPoint(Real2D x) const -> bool   { return std::abs(x.norm() - radius + dt) < eps; }
        auto RingDeltaFunc::myName()                      const -> Str { return "ring Dirac-δ"; }

        auto RingDeltaFunc::operator()         (Real2D x) const -> Real   {
            const auto r = x.norm();
            const auto t = radius;

            return delta(r-t, eps);
        }

    }
}
