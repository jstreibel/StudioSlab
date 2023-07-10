//
// Created by joao on 4/11/23.
//


#include "LeadingDelta.h"

namespace R2toR {
    namespace LeadingDelta {
        RingDeltaFunc::Ptr ringDelta1;

        BoundaryCondition::BoundaryCondition(RingDeltaFunc::Ptr ringDelta, Real tf)
                : ringDelta(ringDelta)
                , tf(tf) { }
        void BoundaryCondition::apply(EquationState &function, Real t) const {
            const bool applies = t<tf || tf<0;
            const bool exist = ringDelta != nullptr;
            const bool should = applies && exist;

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




        auto OutputBuilder::buildOpenGLOutput() -> R2toR::OutputOpenGL * {
            const Real phiMin = **phiMinPlot;
            const Real phiMax = **phiMaxPlot;

            const auto &p = Numerics::Allocator::GetInstance().getNumericParams();
            const Real L = p.getL();
            const Real xLeft = p.getxLeft();
            const Real xRight = xLeft + L;
            const auto dx = 0; // L*2.5/10;

            return new OutGL(ringDelta1, xLeft-dx, xRight+dx, xLeft-dx, xRight+dx, phiMin, phiMax);
        }

        OutputBuilder::OutputBuilder() : R2toR::OutputSystem::Builder("Leading-delta", "Leading delta output builder")  {
            interface->addParameters({phiMinPlot, phiMaxPlot});
        }


        Builder::Builder() : SimulationBuilder("Leading Delta", "simulation builder for (2+1)-d signum-Gordon shockwave as the trail of a driving delta.",
                                               BuilderBasePtr(new LeadingDelta::OutputBuilder)) {
            interface->addParameters({&W_0, &eps, &deltaDuration});
        }
        auto Builder::notifyCLArgsSetupFinished()    ->       void {
            InterfaceOwner::notifyCLArgsSetupFinished();

            auto &p = const_cast<NumericParams&>(Numerics::Allocator::GetInstance().getNumericParams());
            const Real L = p.getL();
            const Real dt = p.getdt();
            const auto W₀ = *W_0;
            const auto C₂ = W₀; // this is C_n from our 2023 shockwave paper, with n=2.

            p.sett(L*.5 - *eps);

            drivingFunc = std::make_shared<RingDeltaFunc>(*eps, C₂, dt);
            ringDelta1 = drivingFunc;
            Allocator::GetInstanceSuper<LeadingDelta::Allocator>().setDrivingFunction(drivingFunc);
        }
        auto Builder::getBoundary()            const -> const void * { return new BoundaryCondition(drivingFunc); }
        auto Builder::registerAllocator()      const ->       void   { Allocator::Initialize<LeadingDelta::Allocator>(); }
    }
}



