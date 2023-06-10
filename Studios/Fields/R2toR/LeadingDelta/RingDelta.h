//
// Created by joao on 08/06/23.
//

#ifndef STUDIOSLAB_RINGDELTA_H
#define STUDIOSLAB_RINGDELTA_H

#include "Common/Types.h"

#include "Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Mappings/R2toR/Model/FunctionsCollection/R2ToRRegularDelta.h"
#include "Mappings/RtoR/Model/FunctionsCollection/RegularDiracDelta.h"
#include "Mappings/R2toR/Model/FunctionsCollection/FunctionAzimuthalSymmetry.h"

namespace R2toR {
    namespace LeadingDelta {



        class SpecialRingDelta : public Function {
        protected:
            const Real eps, a;
            Real radius;
        public:
            typedef std::shared_ptr<SpecialRingDelta> Ptr;

            auto getEps   ()                   const -> Real;
            auto getA     ()                   const -> Real;
            auto getRadius()                   const -> Real;
            virtual auto setRadius(Real _radius)     -> void;
            auto domainContainsPoint(Real2D x) const -> bool override;

            SpecialRingDelta(Real eps, Real a);

        };



        class AzimuthDelta : public SpecialRingDelta, public FunctionAzimuthalSymmetry {
        public:
            AzimuthDelta(Real eps, Real height);
            auto setRadius(Real _radius)    -> void override;
            auto operator()(Real2D x) const -> Real override;
        };



        class Delta_r : public SpecialRingDelta {
            RtoR::RegularDiracDelta delta;
            Real dt;
        public:
            /**
             * Constructor
             * @param eps the delta 'ϵ' parameter;
             * @param height the multiplier in front of the delta.
             * @param dt the minimum 't' to consider (because 't' appears in a denominator). Recommended value
             * is the actual simulation timestep.
             */
            Delta_r(Real eps, Real height, Real dt);
            auto domainContainsPoint(Real2D x) const -> bool override;
            auto operator()         (Real2D x) const -> Real override;

            String myName() const override;
        };
    }
}

#endif //STUDIOSLAB_RINGDELTA_H
