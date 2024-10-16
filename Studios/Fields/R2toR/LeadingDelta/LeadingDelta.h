//
// Created by joao on 4/11/23.
//

#ifndef STUDIOSLAB_LEADINGDELTA_H
#define STUDIOSLAB_LEADINGDELTA_H



#include "Models/KleinGordon/R2toR/R2toRBuilder.h"
#include "Models/KleinGordon/R2toR/BoundaryConditions/R2ToRBoundaryCondition.h"

#include "Monitor.h"
#include "RingDeltaFunc.h"

namespace Slab::Math::R2toR {
    namespace LeadingDelta {


        extern RingDeltaFunc::Ptr ringDelta1;


        class BoundaryCondition : public Core::BoundaryConditions<R2toR::EquationState> {
            RingDeltaFunc::Ptr ringDelta;
            Real tf;
            bool deltaSpeedOp;
        public:
            explicit BoundaryCondition(const R2toR::EquationState *prototype,
                                       RingDeltaFunc::Ptr ringDelta,
                                       Real tf, bool deltaOperatesOnSpeed);
            void apply(EquationState &function, Real t) const override;
        };


        class Builder : public R2toR::Builder {
            RealParameter      W_0              = RealParameter(0.1, "W,W_0",
                                                            "The height of corresponding analytic shockwave, "
                                                            "which determines scale of the delta as "
                                                            "C_n=(n-1)/2 W(0) with n=2.");
            RealParameter      eps              = RealParameter(0.1, "eps",
                                                            "Half the base width of regularized delta;");
            RealParameter      deltaDuration    = RealParameter(-1, "delta_duration",
                                                            "The duration of regularized delta. Negative "
                                                            "values mean forever;");

            RingDeltaFunc::Ptr drivingFunc;

        protected:
            auto buildOpenGLOutput() -> R2toR::OutputOpenGL * override;

        public:
            Builder();

            auto notifyCLArgsSetupFinished() -> void   override;

            auto getBoundary()               -> void * override;

            auto suggestFileName() const -> Str override;


        };
    }
}


#endif //STUDIOSLAB_LEADINGDELTA_H
