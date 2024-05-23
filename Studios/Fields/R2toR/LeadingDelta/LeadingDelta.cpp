//
// Created by joao on 4/11/23.
//


#include "LeadingDelta.h"
#include "Monitor.h"

namespace Slab::Math::R2toR {
    namespace LeadingDelta {
        RingDeltaFunc::Ptr ringDelta1;

        BoundaryCondition::BoundaryCondition(const R2toR::EquationState *prototype,
                                             RingDeltaFunc::Ptr ringDelta,
                                             Real tf,
                                             bool deltaOperatesOnSpeed)
                : Core::BoundaryConditions<R2toR::EquationState>(*prototype)
                , ringDelta(ringDelta)
                , tf(tf)
                , deltaSpeedOp(deltaOperatesOnSpeed) { }
        void BoundaryCondition::apply(EquationState &function, Real t) const {
            const bool applyDelta = t<tf || tf<0;

            if (t == 0) {
                RtoR::NullFunction nullFunction;
                R2toR::FunctionAzimuthalSymmetry fullNull(&nullFunction, 1, 0, 0, false);

                function.setPhi(fullNull);
                function.setDPhiDt(fullNull);
            } else {
                if(!applyDelta) {
                    auto a = ringDelta->getA();
                    ringDelta->setA(a*0.991);
                }

                auto &ϕ =  function.getPhi();
                auto &ϕₜ = function.getDPhiDt();

                if(deltaSpeedOp) {
                    ringDelta->setRadius(t);
                    ringDelta->renderToDiscreteFunction(&ϕₜ);
                } else {
                    const bool AS_THETA = false;

                    if(AS_THETA) {
                        const auto radius = t;

                        ringDelta->setRadius(radius);
                        ringDelta->renderToDiscreteFunction(&ϕ);

                        auto a = ringDelta->getA();
                        ringDelta->setA(0);
                        ringDelta->renderToDiscreteFunction(&ϕₜ);
                        ringDelta->setA(a);
                    } else {
                        const auto eps = ringDelta->getEps();
                        const auto radius = t;

                        ringDelta->setRadius(radius);
                        ringDelta->renderToDiscreteFunction(&ϕ);

                        auto a = ringDelta->getA();
                        ringDelta->setA(0);

                        ringDelta->setRadius(radius + eps);
                        ringDelta->renderToDiscreteFunction(&ϕ);
                        ringDelta->setRadius(radius);

                        ringDelta->renderToDiscreteFunction(&ϕₜ);

                        ringDelta->setA(a);
                    }
                }
            }
        }


        Builder::Builder() : R2toR::Builder("Leading Delta", "simulation builder for (2+1)-d "
                                                                         "signum-Gordon shockwave as the "
                                                                         "trail of a driving delta.") {
            interface->addParameters({&W_0, &eps, &deltaDuration});
        }
        auto Builder::notifyCLArgsSetupFinished()    ->       void {
            InterfaceOwner::notifyCLArgsSetupFinished();

            auto &p = simulationConfig.numericConfig;
            const Real L = p.getL();
            const Real dt = p.getdt();
            const auto W₀ = *W_0;
            auto coef = W₀;

            bool asTheta = deltaDuration<.0;
            if(!asTheta) coef *= 2*eps;

            // p.sett(L*.5 - eps);
            drivingFunc = std::make_shared<RingDeltaFunc>(*eps, coef, dt, asTheta);
            ringDelta1 = drivingFunc;
        }

        auto Builder::getBoundary() -> void * {
            auto eqState = (R2toR::EquationState*)newFieldState();
            return new BoundaryCondition(eqState, drivingFunc, *deltaDuration, false);
        }

        auto Builder::buildOpenGLOutput() -> OutputOpenGL * {
            return new OutGL(simulationConfig.numericConfig, ringDelta1, -1, 1);
        }

        Str Builder::suggestFileName() const {
            auto fname = VoidBuilder::suggestFileName();

            return fname + " " + interface->toString({"W", "eps", "delta_duration"});
        }


    }
}



