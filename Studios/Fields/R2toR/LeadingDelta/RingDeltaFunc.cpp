//
// Created by joao on 08/06/23.
//

#include "RingDeltaFunc.h"
#include "Math/Function/Maps/R2toR/Model/R2toRDiscreteFunctionGPU.h"

namespace R2toR {
    namespace LeadingDelta {


        RingDeltaFunc::RingDeltaFunc(Real eps, Real a, Real dt, bool asTheta)
        : eps(eps), a(a), dt(dt), radius(0), asTheta(asTheta) {  }

        auto RingDeltaFunc::getEps()                      const -> Real   { return eps;       }
        auto RingDeltaFunc::setEps(Real _eps)                   -> void   { eps = _eps;       }

        auto RingDeltaFunc::getA()                        const -> Real   { return a;         }
        auto RingDeltaFunc::setA(Real _a)                        -> void  { a = _a;           }

        auto RingDeltaFunc::setRadius(Real _radius)             -> void   { radius = _radius; }
        auto RingDeltaFunc::getRadius()                   const -> Real   { return radius;    }

        auto RingDeltaFunc::domainContainsPoint(Real2D x) const -> bool   {
            // const auto rad = radius + dt;
            const auto rad = radius;
            return std::abs(x.norm() - rad) < eps;
        }
        auto RingDeltaFunc::myName()                      const -> Str { return "ring Dirac-δ"; }

        auto RingDeltaFunc::operator()         (Real2D x) const -> Real   {
            const auto r = x.norm();
            const auto t = radius;

            return a*delta(r-t, eps);
        }

    }
}
