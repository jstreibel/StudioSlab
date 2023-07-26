//
// Created by joao on 08/06/23.
//

#ifndef STUDIOSLAB_RINGDELTAFUNC_H
#define STUDIOSLAB_RINGDELTAFUNC_H

#include "Common/Types.h"

#include "Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Mappings/RtoR/Model/FunctionsCollection/RegularDiracDelta.h"

#include "Mappings/R2toR/Model/FunctionsCollection/R2ToRRegularDelta.h"
#include "Mappings/R2toR/Model/FunctionsCollection/FunctionAzimuthalSymmetry.h"

#define delta(r, eps) deltaRect(r, eps)

namespace R2toR {
    namespace LeadingDelta {

        typedef Base::DiscreteFunction<Real2D, Real> ArbFunc;

        class RingDeltaFunc : public Function {
        protected:
            Real eps, a, dt;
            Real radius;

        public:
            typedef std::shared_ptr<RingDeltaFunc> Ptr;

            auto getEps   ()                   const -> Real;
            auto getA     ()                   const -> Real;
            auto setA     (Real a)                   -> void;
            auto getRadius()                   const -> Real;
            auto setRadius(Real _radius)             -> void;
            auto domainContainsPoint(Real2D x) const -> bool override;
            auto myName()                      const -> Str  override ;

            /**
             * Constructor
             * @param eps the delta 'ϵ' parameter;
             * @param height the multiplier in front of the delta.
             * @param dt the minimum 't' to consider (because 't' appears in a denominator). Recommended value
             * is the actual simulation timestep.
             */
            RingDeltaFunc(Real eps, Real a, Real dt);
            auto operator()(Real2D x) const -> Real override;

            bool renderToDiscreteFunction(ArbFunc *toFunc) const override;
        };

    }
}

#endif //STUDIOSLAB_RINGDELTAFUNC_H
