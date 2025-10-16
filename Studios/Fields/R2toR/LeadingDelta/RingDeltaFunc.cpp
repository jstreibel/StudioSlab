//
// Created by joao on 08/06/23.
//

#include "RingDeltaFunc.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionGPU.h"

namespace Studios::Fields::R2toRLeadingDelta {

    RingDeltaFunc::RingDeltaFunc(DevFloat eps, DevFloat a, DevFloat dt, bool asTheta)
    : eps(eps), a(a), dt(dt), radius(0), asTheta(asTheta) {  }

    auto RingDeltaFunc::getEps()                      const -> DevFloat   { return eps;       }
    auto RingDeltaFunc::setEps(DevFloat _eps)                   -> void   { eps = _eps;       }

    auto RingDeltaFunc::getA()                        const -> DevFloat   { return a;         }
    auto RingDeltaFunc::setA(DevFloat _a)                        -> void  { a = _a;           }

    auto RingDeltaFunc::setRadius(DevFloat _radius)             -> void   { radius = _radius; }
    auto RingDeltaFunc::getRadius()                   const -> DevFloat   { return radius;    }

    auto RingDeltaFunc::domainContainsPoint(Real2D x) const -> bool   {
        // const auto rad = radius + dt;
        const auto rad = radius;
        return std::abs(x.norm() - rad) < eps;
    }
    auto RingDeltaFunc::generalName()                 const -> Str { return "ring Dirac-δ"; }

    auto RingDeltaFunc::operator()         (Real2D x) const -> DevFloat   {
        const auto r = x.norm();
        const auto t = radius;

        return a*delta(r-t, eps);
    }

    #if !USE_CUDA
    bool RingDeltaFunc::renderToNumericFunction(ArbFunc *toFunc) const
    {
        Slab::Core::Log::Error("RingDeltaFunc::renderToNumericFunction only implemented in GPU. Current compilation has no GPU support");
        NOT_IMPLEMENTED_CLASS_METHOD
    }
    #endif

}