//
// Created by joao on 4/11/23.
//


#include "LeadingDelta.h"

namespace R2toR {
    namespace LeadingDelta {
        RingDeltaFunc::Ptr ringDelta1;

        BoundaryCondition::BoundaryCondition(const R2toR::EquationState *prototype, RingDeltaFunc::Ptr ringDelta, Real tf)
        : Base::BoundaryConditions<R2toR::EquationState>(*prototype)
        , ringDelta(ringDelta)
        , tf(tf) { }
        void BoundaryCondition::apply(EquationState &function, Real t) const {
            const bool applies = t<tf || tf<0;
            const bool exist = ringDelta != nullptr;
            const bool should = applies && exist;

            Log::Debug() << "Applying BC" << Log::Flush;

            if (t == 0) {
                RtoR::NullFunction nullFunction;
                R2toR::FunctionAzimuthalSymmetry fullNull(&nullFunction, 1, 0, 0, false);

                function.setPhi(fullNull);
                function.setDPhiDt(fullNull);
            } else if (should) {
                ringDelta->setRadius(t);

                auto &ϕₜ = function.getDPhiDt();
                ringDelta->renderToDiscreteFunction(&ϕₜ);
            }
        }


        Builder::Builder() : R2toR::Builder("Leading Delta", "simulation builder for (2+1)-d "
                                                                         "signum-Gordon shockwave as the "
                                                                         "trail of a driving delta.") {
            interface->addParameters({&W_0, &eps, &deltaDuration});
        }
        auto Builder::notifyCLArgsSetupFinished()    ->       void {
            InterfaceOwner::notifyCLArgsSetupFinished();

            auto &p = numericParams;
            const Real L = p.getL();
            const Real dt = p.getdt();
            const auto W₀ = *W_0;
            const auto C₂ = W₀; // this is C_n from our 2023 shockwave paper, with n=2.

            p.sett(L*.5 - *eps);

            drivingFunc = std::make_shared<RingDeltaFunc>(*eps, C₂, dt);
            ringDelta1 = drivingFunc;
        }

        auto Builder::getBoundary() -> void * {
            auto eqState = (R2toR::EquationState*)newFieldState();
            return new BoundaryCondition(eqState, drivingFunc);
        }

        auto Builder::buildOpenGLOutput() -> OutputOpenGL * {
            return new OutGL(numericParams, ringDelta1, -1, 1);
        }


    }
}



