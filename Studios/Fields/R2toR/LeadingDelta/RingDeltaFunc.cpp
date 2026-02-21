//
// Created by joao on 08/06/23.
//

#include "RingDeltaFunc.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionGPU.h"

namespace Studios::Fields::R2toRLeadingDelta {

    FRingDeltaFunc::FRingDeltaFunc(DevFloat eps, DevFloat a, DevFloat dt, bool asTheta)
    : eps(eps), a(a), dt(dt), radius(0), asTheta(asTheta) {  }

    auto FRingDeltaFunc::getEps()                      const -> DevFloat   { return eps;       }
    auto FRingDeltaFunc::setEps(DevFloat _eps)                   -> void   { eps = _eps;       }

    auto FRingDeltaFunc::getA()                        const -> DevFloat   { return a;         }
    auto FRingDeltaFunc::setA(DevFloat _a)                        -> void  { a = _a;           }

    auto FRingDeltaFunc::setRadius(DevFloat _radius)             -> void   { radius = _radius; }
    auto FRingDeltaFunc::getRadius()                   const -> DevFloat   { return radius;    }

    auto FRingDeltaFunc::domainContainsPoint(Real2D x) const -> bool   {
        // const auto rad = radius + dt;
        const auto rad = radius;
        return std::abs(x.norm() - rad) < eps;
    }
    auto FRingDeltaFunc::generalName()                 const -> Str { return "ring Dirac-δ"; }

    auto FRingDeltaFunc::operator()         (Real2D x) const -> DevFloat   {
        const auto r = x.norm();
        const auto t = radius;

        return a*delta(r-t, eps);
    }

    #if !USE_CUDA
    bool FRingDeltaFunc::renderToNumericFunction(ArbFunc *toFunc) const
    {
        Slab::Core::Log::Error("FRingDeltaFunc::renderToNumericFunction only implemented in GPU. Current compilation has no GPU support");
        NOT_IMPLEMENTED_CLASS_METHOD
    }
    #endif

}
