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

        extern SpecialRingDelta *ringDelta1;
        extern Real ring_tf;



        class BoundaryCondition : public Base::BoundaryConditions<R2toR::FieldState> {
            SpecialRingDelta::Ptr ringDelta;
            Real tf;
        public:
            explicit BoundaryCondition(SpecialRingDelta::Ptr ringDelta = nullptr, Real tf=-1)
            : ringDelta(ringDelta), tf(tf) { }
            void apply(FieldState &function, Real t) const override {
                if (t == 0) {
                    RtoR::NullFunction nullFunction;
                    R2toR::FunctionAzimuthalSymmetry fullNull(&nullFunction, 1, 0, 0, false);

                    function.setPhi(fullNull);
                    function.setDPhiDt(fullNull);
                } else if (ringDelta != nullptr && (t<tf || tf<0)) {
                    auto &srd = const_cast<SpecialRingDelta&>(*ringDelta);
                    srd.setRadius(t);

                    auto &dφdt = function.getDPhiDt();

                    function.setDPhiDt(*ringDelta);
                }
            }
        };

        class OutputBuilder : public R2toR::OutputSystem::Builder {
        protected:
            auto buildOpenGLOutput() -> R2toR::OutputOpenGL * override {
                const Real phiMin = -.2;
                const Real phiMax =  1.2;

                const auto &p = Numerics::Allocator::getInstance().getNumericParams();
                const Real L = p.getL();
                const Real xLeft = p.getxLeft();
                const Real xRight = xLeft + L;
                const auto dx = 0; // L*2.5/10;

                return new OutGL(xLeft-dx, xRight+dx, xLeft-dx, xRight+dx, phiMin, phiMax);
            }
        };

        class Builder : public SimulationBuilder {
            RealParameter W_0 = RealParameter(0.1, "W_0,W", "The height of corresponding analytic shockwave, "
                                                            "which determines scale of the delta as "
                                                            "C_n=(n-1)/2 W(0) with n=2.");
            RealParameter eps = RealParameter(0.1, "eps",   "Half the base width of regularized delta;");
            RealParameter deltaDuration = RealParameter(-1, "delta_duration", "The duration of "
                                                                                  "regularized delta. Negative "
                                                                                  "values mean forever;");

            SpecialRingDelta::Ptr drivingFunc;

        public:
            Builder() : SimulationBuilder("(2+1)-d Shockwave as trail of a driving delta.",
                                          "ldd", new OutputBuilder) {
                addParameters({W_0, eps, deltaDuration});
            }

            void setup(CLVariablesMap vm) override {
                Interface::setup(vm);

                auto &p = const_cast<NumericParams&>(Numerics::Allocator::getInstance().getNumericParams());
                const Real L = p.getL();
                const Real dt = p.getdt();

                p.sett(L*.5 - *eps *2);

                const auto C_2 = *W_0 / 2.0; // this is C_n from our 2023 shockwave paper, with n=2.

                if (0)     drivingFunc = std::make_shared<AzimuthDelta>(*eps, C_2);
                else       drivingFunc = std::make_shared<Delta_r>(*eps, C_2, dt);
            }

            auto getBoundary() const -> const void * override {


                return new BoundaryCondition(drivingFunc);
            }

            auto registerAllocator() const -> void override {
                auto dt = Numerics::Allocator::getInstance().getNumericParams().getdt();
                Function::Ptr 𝒻 = std::make_unique<Delta_r>(*eps, *W_0, dt);

                LeadingDelta::Allocator::Choose(𝒻);
            }
        };
    }
}


#endif //STUDIOSLAB_LEADINGDELTA_H
