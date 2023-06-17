//
// Created by joao on 08/06/23.
//

#include "RingDelta.h"
#include "Mappings/R2toR/Model/R2ToRFunctionArbitraryGPU.h"

namespace R2toR {
    namespace LeadingDelta {


        RingDelta::RingDelta(Real eps, Real a, Real dt) : eps(eps), a(a), dt(dt), radius(0) {}

        auto RingDelta::getEps()                      const -> Real { return eps; }
        auto RingDelta::getA()                        const -> Real { return a; }

        auto RingDelta::setRadius(Real _radius)             -> void { radius = _radius; }
        auto RingDelta::getRadius()                   const -> Real { return radius; }

        auto RingDelta::domainContainsPoint(Real2D x) const -> bool { return abs(x.norm()-radius+dt)<eps; }
        String RingDelta::myName() const { return "ring Dirac-δ"; }

        auto RingDelta::operator()         (Real2D x) const -> Real {
            //const auto r = x.norm();
            //const auto t = radius+dt; // t appears in denominator so can't be zero.
            //return (r/t) * deltaTri(r-t, eps);

            return deltaTri(x.norm()-radius, eps);
        }
        bool R2toR::LeadingDelta::RingDelta::renderToDiscreteFunction(Base::ArbitraryFunction<Real2D, Real> *toFunc) const {
            // DeviceVector toRender = toFunc->getSpace().getDeviceData();

            return true;
        }
    }
}
