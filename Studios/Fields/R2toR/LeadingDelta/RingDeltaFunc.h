//
// Created by joao on 08/06/23.
//

#ifndef STUDIOSLAB_RINGDELTAFUNC_H
#define STUDIOSLAB_RINGDELTAFUNC_H

#include "Utils/Types.h"

#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/RegularDiracDelta.h"

#include "Math/Function/R2toR/Model/FunctionsCollection/R2ToRRegularDelta.h"
#include "Math/Function/R2toR/Model/FunctionsCollection/FunctionAzimuthalSymmetry.h"
#include "Math/Formalism/Categories.h"
#include "Math/Function/R2toR/Model/R2toRFunction.h"

#define delta(r, eps) Slab::Math::deltaRect(r, eps)


namespace Studios::Fields::R2toRLeadingDelta {

    using Real = Slab::Real;
    using Real2D = Slab::Math::Real2D;
    using Str = Slab::Str;

    using ArbFunc = Slab::Math::Base::NumericFunction<Slab::Math::Real2D, Slab::Real>;

    class RingDeltaFunc : public Slab::Math::R2toR::Function {
    protected:
        Slab::Real eps, a, dt;
        Slab::Real radius;
        bool asTheta;

    public:
        auto getEps   ()                   const -> Real;
        auto setEps   (Real eps)                 -> void;
        auto getA     ()                   const -> Real;
        auto setA     (Real a)                   -> void;
        auto getRadius()                   const -> Real;
        auto setRadius(Real _radius)             -> void;
        auto domainContainsPoint(Real2D x) const -> bool override;

        Str generalName() const override;

        /**
         * Constructor
         * @param eps the delta 'ϵ' parameter;
         * @param height the multiplier in front of the delta.
         * @param dt the minimum 't' to consider (because 't' appears in a denominator). Recommended value
         * is the actual simulation timestep.
         */
        RingDeltaFunc(Real eps, Real a, Real dt, bool asTheta);
        auto operator()(Real2D x) const -> Real override;

        bool renderToNumericFunction(ArbFunc *toFunc) const override;
    };

}


#endif //STUDIOSLAB_RINGDELTAFUNC_H
