//
// Created by joao on 4/11/23.
//


#include "LeadingDelta.h"



namespace R2toR {
    namespace LeadingDelta {
        SpecialRingDelta::Ptr ringDelta1;




        BoundaryCondition::BoundaryCondition(SpecialRingDelta::Ptr ringDelta, Real tf)
                : ringDelta(ringDelta), tf(tf) { }
        void BoundaryCondition::apply(FieldState &function, Real t) const {
            const bool applies = t<tf || tf<0;
            const bool exist = ringDelta != nullptr;
            const bool should = applies && exist;

            if (t == 0) {
                RtoR::NullFunction nullFunction;
                R2toR::FunctionAzimuthalSymmetry fullNull(&nullFunction, 1, 0, 0, false);

                function.setPhi(fullNull);
                function.setDPhiDt(fullNull);
            } else if (should) {
                auto &srd = const_cast<SpecialRingDelta&>(*ringDelta);
                srd.setRadius(t);

                auto &dφdt = function.getDPhiDt();
                dφdt = *ringDelta;

                // function.setDPhiDt();
            }
        }




        auto OutputBuilder::buildOpenGLOutput() -> R2toR::OutputOpenGL * {
            const Real phiMin = -.2;
            const Real phiMax =  1.2;

            const auto &p = Numerics::Allocator::getInstance().getNumericParams();
            const Real L = p.getL();
            const Real xLeft = p.getxLeft();
            const Real xRight = xLeft + L;
            const auto dx = 0; // L*2.5/10;

            return new OutGL(ringDelta1, xLeft-dx, xRight+dx, xLeft-dx, xRight+dx, phiMin, phiMax);
        }




        Builder::Builder() : SimulationBuilder("ldd,(2+1)-d Shockwave as trail of a driving delta.",
                                               BuilderBasePtr(new LeadingDelta::OutputBuilder)) {
            interface->addParameters({W_0, eps, deltaDuration});
        }
        auto Builder::notifyCLArgsSetupFinished()    -> void {
            InterfaceOwner::notifyCLArgsSetupFinished();

            auto &p = const_cast<NumericParams&>(Numerics::Allocator::getInstance().getNumericParams());
            const Real L = p.getL();
            const Real dt = p.getdt();

            p.sett(L*.5 - **eps *2);

            const auto C_2 = **W_0 / 2.0; // this is C_n from our 2023 shockwave paper, with n=2.

            if (0)     drivingFunc = std::make_shared<AzimuthDelta>(**eps, C_2);
            else       drivingFunc = std::make_shared<Delta_r>(**eps, C_2, dt);

            ringDelta1 = drivingFunc;
        }
        auto Builder::getBoundary()            const -> const void * { return new BoundaryCondition(drivingFunc); }
        auto Builder::registerAllocator()      const -> void {
            LeadingDelta::Allocator::Choose();
        }
    }
}



