//
// Created by joao on 4/11/23.
//


#include "LeadingDelta.h"

namespace R2toR {
    namespace LeadingDelta {
        RingDeltaFunc::Ptr ringDelta1;

        BoundaryCondition::BoundaryCondition(RingDeltaFunc::Ptr ringDelta,
                                             Real tf,
                                             bool deltaOperatesOnSpeed)
                : ringDelta(ringDelta)
                , tf(tf)
                , deltaSpeedOp(deltaOperatesOnSpeed) { }
        void BoundaryCondition::apply(EquationState &function, Real t) const {
            const bool applyDelta = t<tf || tf<0;

            if (t == 0) {
                RtoR::NullFunction nullFunction;
                R2toR::FunctionAzimuthalSymmetry fullNull(&nullFunction, 1, 0, 0, false);

                function.setPhi(fullNull);
                function.setDPhiDt(fullNull);
            } else if (applyDelta) {
                auto &ϕ =  function.getPhi();
                auto &ϕₜ = function.getDPhiDt();

                if(deltaSpeedOp) {
                    ringDelta->setRadius(t);
                    ringDelta->renderToDiscreteFunction(&ϕₜ);
                } else {
                    const auto eps = ringDelta->getEps();
                    const auto radius = t + 0.9*eps;

                    ringDelta->setRadius(radius);
                    ringDelta->renderToDiscreteFunction(&ϕ);

                    auto a = ringDelta->getA();
                    ringDelta->setA(0);

                    ringDelta->setRadius(radius+2*eps);
                    ringDelta->renderToDiscreteFunction(&ϕ);
                    ringDelta->setRadius(radius);

                    ringDelta->renderToDiscreteFunction(&ϕₜ);

                    ringDelta->setA(a);
                }
            }
        }




        auto OutputBuilder::buildOpenGLOutput() -> R2toR::OutputOpenGL * {
            const Real phiMin = **phiMinPlot;
            const Real phiMax = **phiMaxPlot;

            const auto &p = Numerics::Allocator::GetInstance().getNumericParams();
            const Real L = p.getL();
            const Real xLeft = p.getxLeft();
            const Real xRight = xLeft + L;
            const auto dx = 0; // L*2.5/10;

            return new OutGL(p, ringDelta1, xLeft-dx, xRight+dx, xLeft-dx, xRight+dx, phiMin, phiMax);
        }

        OutputBuilder::OutputBuilder() : R2toR::OutputSystem::Builder("Leading-delta", "Leading delta output builder")  {
            interface->addParameters({phiMinPlot, phiMaxPlot});
        }


        Builder::Builder() : SimulationBuilder("Leading Delta", "simulation builder for (2+1)-d signum-Gordon shockwave as the trail of a driving delta.",
                                               BuilderBasePtr(new LeadingDelta::OutputBuilder)) {
            interface->addParameters({&W_0, &eps, &deltaDuration, &deltaOperatesOnSpeed});
        }
        auto Builder::notifyCLArgsSetupFinished()    ->       void {
            InterfaceOwner::notifyCLArgsSetupFinished();

            auto &p = const_cast<NumericParams&>(Numerics::Allocator::GetInstance().getNumericParams());
            const Real L = p.getL();
            const Real dt = p.getdt();
            const Real ϵ = *eps;
            const auto W₀ = *W_0;
            auto a = W₀;

            if(!*deltaOperatesOnSpeed) a *= 2*ϵ; // TODO isso eh gambiarra, pq nao eh de fato uma delta que opera no campo.

            p.sett(L*.5 - 10*ϵ);

            drivingFunc = std::make_shared<RingDeltaFunc>(ϵ, a, dt);
            ringDelta1 = drivingFunc;
            Allocator::GetInstanceSuper<LeadingDelta::Allocator>().setDrivingFunction(drivingFunc);
        }
        auto Builder::getBoundary()            const -> const void * {
            return new BoundaryCondition(drivingFunc, *deltaDuration, *deltaOperatesOnSpeed); }
        auto Builder::registerAllocator()      const ->       void   { Allocator::Initialize<LeadingDelta::Allocator>(); }
    }
}



