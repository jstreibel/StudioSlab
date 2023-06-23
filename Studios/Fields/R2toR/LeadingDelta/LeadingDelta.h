//
// Created by joao on 4/11/23.
//

#ifndef STUDIOSLAB_LEADINGDELTA_H
#define STUDIOSLAB_LEADINGDELTA_H



#include <Mappings/R2toR/Controller/R2ToR_SimulationBuilder.h>
#include "Mappings/R2toR/Model/BoundaryConditions/R2ToRBoundaryCondition.h"

#include "Monitor.h"
#include "RingDelta.h"
#include "DrivenEquation.h"
#include "Allocator.h"


namespace R2toR {
    namespace LeadingDelta {


        extern RingDelta::Ptr ringDelta1;



        class BoundaryCondition : public Base::BoundaryConditions<R2toR::FieldState> {
            RingDelta::Ptr ringDelta;
            Real tf;
        public:
            explicit BoundaryCondition(RingDelta::Ptr ringDelta = nullptr, Real tf=-1);
            void apply(FieldState &function, Real t) const override;
        };



        class OutputBuilder : public R2toR::OutputSystem::Builder {
        protected:
            auto buildOpenGLOutput() -> R2toR::OutputOpenGL * override;
        };



        class Builder : public SimulationBuilder {
            RealParameter::Ptr      W_0              = RealParameter::New(0.1, "W,W_0",
                                                            "The height of corresponding analytic shockwave, "
                                                            "which determines scale of the delta as "
                                                            "C_n=(n-1)/2 W(0) with n=2.");
            RealParameter::Ptr      eps              = RealParameter::New(0.1, "eps",
                                                            "Half the base width of regularized delta;");
            RealParameter::Ptr      deltaDuration    = RealParameter::New(-1, "delta_duration",
                                                            "The duration of regularized delta. Negative "
                                                            "values mean forever;");
            RingDelta::Ptr drivingFunc;

        public:
            Builder();
            auto notifyCLArgsSetupFinished()               -> void         override;
            auto getBoundary()          const -> const void * override;
            auto registerAllocator()    const -> void         override;
        };
    }
}


#endif //STUDIOSLAB_LEADINGDELTA_H
