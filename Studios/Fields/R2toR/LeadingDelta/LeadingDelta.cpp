//
// Created by joao on 4/11/23.
//


#include "LeadingDelta.h"

#include <utility>
#include "Monitor.h"

namespace Studios::Fields::R2toRLeadingDelta {
    TPointer<RingDeltaFunc> ringDelta1;

    BoundaryCondition::BoundaryCondition(const TPointer<const R2toR::EquationState>& prototype,
                                         TPointer<RingDeltaFunc> ringDelta,
                                         Real tf,
                                         bool deltaOperatesOnSpeed)
            : Slab::Math::Base::BoundaryConditions(prototype)
            , ringDelta(std::move(ringDelta))
            , tf(tf)
            , deltaSpeedOp(deltaOperatesOnSpeed) { }

    void BoundaryCondition::Apply(Slab::Math::Base::EquationState &state, Real t) const {
        const bool applyDelta = t<tf || tf<0;

        auto stateKG = dynamic_cast<Slab::Math::R2toR::EquationState&>(state);

        if (t == 0) {
            RtoR::NullFunction nullFunction;
            R2toR::FunctionAzimuthalSymmetry fullNull(&nullFunction, 1, 0, 0, false);

            stateKG.SetPhi(fullNull);
            stateKG.SetDPhiDt(fullNull);
        } else {
            if(!applyDelta) {
                auto a = ringDelta->getA();
                ringDelta->setA(a*0.991);
            }

            auto &ϕ =  stateKG.getPhi();
            auto &ϕₜ = stateKG.getDPhiDt();

            if(deltaSpeedOp) {
                ringDelta->setRadius(t);
                ringDelta->renderToNumericFunction(&ϕₜ);
            } else {
                const bool AS_THETA = true;

                if(AS_THETA) {
                    const auto radius = t;

                    ringDelta->setRadius(radius);
                    ringDelta->renderToNumericFunction(&ϕ);

                    auto a = ringDelta->getA();
                    ringDelta->setA(0);
                    ringDelta->renderToNumericFunction(&ϕₜ);
                    ringDelta->setA(a);
                } else {
                    const auto eps = ringDelta->getEps();
                    const auto radius = t;

                    ringDelta->setRadius(radius);
                    ringDelta->renderToNumericFunction(&ϕ);

                    auto a = ringDelta->getA();
                    ringDelta->setA(0);

                    ringDelta->setRadius(radius + eps);
                    ringDelta->renderToNumericFunction(&ϕ);
                    ringDelta->setRadius(radius);

                    ringDelta->renderToNumericFunction(&ϕₜ);

                    ringDelta->setA(a);
                }
            }
        }
    }


    Builder::Builder(bool do_register) : Models::KGR2toR::Builder("Leading Delta", "simulation builder for (2+1)-d "
                                                                     "signum-Gordon shockwave as the "
                                                                     "trail of a driving delta.", false) {
        Interface->AddParameters({&W_0, &eps, &deltaDuration});

        if(do_register) RegisterToManager();
    }
    auto Builder::NotifyCLArgsSetupFinished()    ->       void {
        FCommandLineInterfaceOwner::NotifyCLArgsSetupFinished();

        auto &p = *kg_numeric_config;
        const Real L = p.GetL();
        const Real dt = p.Getdt();
        const auto W₀ = *W_0;
        auto coef = W₀;

        bool asTheta = deltaDuration<.0;
        if(!asTheta) coef *= 2*eps;

        // p.sett(L*.5 - eps);
        drivingFunc = New<RingDeltaFunc>(*eps, coef, dt, asTheta);
        ringDelta1 = drivingFunc;
    }

    auto Builder::getBoundary() -> TPointer<Base::BoundaryConditions> {
        auto eqStatePrototype = newFieldState();

        return New<BoundaryCondition>(eqStatePrototype, drivingFunc, *deltaDuration, false);
    }

    auto Builder::buildOpenGLOutput() -> Models::KGR2toR::OutputOpenGL * {
        return new OutGL(kg_numeric_config->getn(), ringDelta1);
    }

    Str Builder::SuggestFileName() const {
        auto fname = NumericalRecipe::SuggestFileName();

        return fname + " " + Interface->ToString({"W", "eps", "delta_duration"});
    }

    void *Builder::getHamiltonian() {
        return nullptr;
    }


}


