//
// Created by joao on 08/06/23.
//

#include "RingDelta.h"


namespace R2toR {
    namespace LeadingDelta {

        inline Real z(const Real r, const Real t) {
            // return r-t;
            return .25 * (r * r - t * t);
        }


        SpecialRingDelta::SpecialRingDelta(Real eps, Real a) : eps(eps), a(a), radius(0) {}
        auto SpecialRingDelta::getEps()                      const -> Real { return eps; }
        auto SpecialRingDelta::getA()                        const -> Real { return a; }
        auto SpecialRingDelta::domainContainsPoint(Real2D x) const -> bool {
            if(0) {
                const auto eps2 = .5 * eps;

                const auto r = x.norm();
                const auto t = radius;

                return r - t > -eps2 && r - t < +eps2;
            }

            const auto r = x.norm();
            const auto t = radius;
            const auto d = r-t;

            return d > -eps
                   && d < +eps;
        }
        auto SpecialRingDelta::setRadius(Real _radius)             -> void { radius = _radius; }
        auto SpecialRingDelta::getRadius()                   const -> Real { return radius; }



        AzimuthDelta::AzimuthDelta(Real eps, Real height)
                : SpecialRingDelta          (eps, height),
                  FunctionAzimuthalSymmetry (new RtoR::RegularDiracDelta(eps, height)) {}
        auto AzimuthDelta::setRadius(Real _radius)    -> void {
            SpecialRingDelta::setRadius(_radius);

            const auto &func = static_cast<const RtoR::RegularDiracDelta &>(this->getRadialFunction());
            auto &delta = const_cast<RtoR::RegularDiracDelta &>(func);
            delta.setTranslation(radius);
        }
        auto AzimuthDelta::operator()(Real2D x) const -> Real {
            return FunctionAzimuthalSymmetry::operator()(x); }


        Delta_r::Delta_r(Real eps, Real height, Real dt)
                : SpecialRingDelta(eps, height),
                  delta           (eps, height, RtoR::RegularDiracDelta::Regularization::Triangle),
                  dt              (dt) {}
        auto Delta_r::domainContainsPoint(Real2D x) const -> bool {
            const auto r = x.norm();
            const auto t = radius;

            #if(0)
            { const auto _z = z(r, t); return _z > -eps && _z < +eps; }
            #endif

            const auto d = r-t;
            return d > -eps
                && d < +eps;
        }
        auto Delta_r::operator()         (Real2D x) const -> Real {
            const auto r = x.norm();
            const auto t = radius+dt; // t appears in denominator so can't be zero.

            return (r/t) * delta(r-t);;
        }


    }
}

