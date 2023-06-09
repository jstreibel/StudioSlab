//
// Created by joao on 4/11/23.
//

#ifndef STUDIOSLAB_LEADINGDELTA_H
#define STUDIOSLAB_LEADINGDELTA_H

#include <Mappings/R2toR/Controller/R2ToR_SimulationBuilder.h>

#include "Mappings/R2toR/Model/BoundaryConditions/R2ToRBoundaryCondition.h"
#include "Phys/Numerics/Allocator.h"
#include "Phys/Graph/PointSetGraph.h"


#include "RingDelta.h"
#include "DrivenEquation.h"
#include "Allocator.h"


namespace R2toR {

    namespace LeadingDelta {

        extern SpecialRingDelta *ringDelta1;
        extern Real ring_tf;



        class OutGL : public R2toR::OutputOpenGL {
            Spaces::PointSet::Ptr totalEnergyData;
            Phys::Graphing::PointSetGraph mTotalEnergyGraph;

            Spaces::PointSet::Ptr numericEnergyData;
            Spaces::PointSet::Ptr analyticEnergyData;
            Phys::Graphing::PointSetGraph mEnergyGraph;

            Spaces::PointSet::Ptr energyRatioData;
            Phys::Graphing::PointSetGraph mEnergyRatioGraph;

        protected:
            auto _out(const OutputPacket &outInfo) -> void override;
        public:
            OutGL(Real xMin, Real xMax, Real yMin, Real yMax, Real phiMin, Real phiMax);
            auto draw() -> void override;
            auto getWindowSizeHint() -> IntPair override;
            auto notifyKeyboard(unsigned char key, int x, int y) -> bool override;
            bool notifyMousePassiveMotion(int x, int y)                  override;
            bool notifyMouseMotion       (int x, int y)                  override;
            bool notifyMouseButton(int button, int dir, int x, int y)    override;
        };



        class BoundaryCondition : public Base::BoundaryConditions<R2toR::FieldState> {
            SpecialRingDelta &ringDelta;
            Real tf;
        public:
            explicit BoundaryCondition(SpecialRingDelta &ringDelta, Real tf=-1) : ringDelta(ringDelta), tf(tf)
            {
                ringDelta1 = &ringDelta;
                ring_tf = tf;
            }
            void apply(FieldState &function, Real t) const override {
                if (t == 0) {
                    RtoR::NullFunction nullFunction;
                    R2toR::FunctionAzimuthalSymmetry fullNull(&nullFunction, 1, 0, 0, false);

                    function.setPhi(fullNull);
                    function.setDPhiDt(fullNull);
                }
            #if false
                else if (t<tf || tf<0){
                    const_cast<SpecialRingDelta&>(ringDelta).setRadius(t);

                    auto &dφdt = function.getDPhiDt();


                    function.setDPhiDt(ringDelta);
                }
            #endif
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
        public:
            Builder() : SimulationBuilder("(2+1)-d Shockwave as trail of a driving delta.",
                                          "ldd", new OutputBuilder) { addParameters({W_0, eps, deltaDuration}); }
            auto getBoundary() const -> const void * override {
                auto &p = const_cast<NumericParams&>(Numerics::Allocator::getInstance().getNumericParams());
                const Real L = p.getL();
                const Real dt = p.getdt();

                p.sett(L*.5 - *eps *2);

                SpecialRingDelta *delta = nullptr;

                const auto C_2 = *W_0 / 2.0; // this is C_n from our 2023 shockwave paper, with n=2.

                if (0)     delta = new AzimuthDelta(*eps, C_2);
                else       delta = new Delta_r(*eps, C_2, dt);

                return new BoundaryCondition(*delta, *deltaDuration);
            }

            auto registerAllocator() const -> void override {
                LeadingDelta::Allocator::Choose(*eps, *W_0, *deltaDuration);
            }
        };
    }
}


#endif //STUDIOSLAB_LEADINGDELTA_H
