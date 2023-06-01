//
// Created by joao on 4/11/23.
//

#ifndef STUDIOSLAB_LEADINGDELTA_H
#define STUDIOSLAB_LEADINGDELTA_H

#include <Fields/Mappings/R2toR/Controller/R2ToRBCInterface.h>

#include "Fields/Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Fields/Mappings/R2toR/Model/FunctionsCollection/R2ToRRegularDelta.h"
#include "Fields/Mappings/R2toR/Model/BoundaryConditions/R2ToRBoundaryCondition.h"
#include "Fields/Mappings/RtoR/Model/FunctionsCollection/RegularDiracDelta.h"
#include "Fields/Mappings/R2toR/Model/FunctionsCollection/FunctionAzimuthalSymmetry.h"
#include "Phys/Numerics/Allocator.h"
#include "Phys/Graph/PointSetGraph.h"

//#define USE_VTK true

#if USE_VTK
#include <vtkSmartPointer.h>
#include <vtkRenderWindowInteractor.h>
#endif

namespace R2toR {

    namespace LeadingDelta {

        class SpecialRingDelta : public Function {
        protected:
            const Real eps, a;
        public:
            const Real getEps() const { return eps; }
            const Real getA() const { return a; }

        protected:

            Real radius;

        public:
            SpecialRingDelta(Real eps, Real a) : eps(eps), a(a), radius(0) { }

            bool domainContainsPoint(Real2D x) const override {
                const auto eps2 = .5*eps;

                const auto r = x.norm();
                const auto t = radius;

                return r-t>-eps2 && r-t<+eps2;
            }

            virtual void setRadius(Real _radius) { radius = _radius; }

            Real getRadius() const { return radius; };
        };


        class AzimuthDelta : public SpecialRingDelta, public FunctionAzimuthalSymmetry {
        public:
            AzimuthDelta(Real eps, Real height)
                    : SpecialRingDelta(eps, height),
                      FunctionAzimuthalSymmetry(new RtoR::RegularDiracDelta(eps, height)) { }

            void setRadius(Real _radius) override {
                SpecialRingDelta::setRadius(_radius);

                const auto &func = static_cast<const RtoR::RegularDiracDelta&>(this->getRadialFunction());
                auto &delta = const_cast<RtoR::RegularDiracDelta&>(func);
                delta.setTranslation(radius);
            }

            Real operator()(Real2D x) const override { return FunctionAzimuthalSymmetry::operator()(x); }
        };


        inline Real Z(const Real r, const Real t) {
            return r-t;
        }
        class ZDelta : public SpecialRingDelta {
            RtoR::RegularDiracDelta delta;

        public:
            ZDelta(Real eps, Real height)
            : SpecialRingDelta(eps, height),
              delta(eps, height, RtoR::RegularDiracDelta::Regularization::Triangle) { }

            [[nodiscard]] bool domainContainsPoint(Real2D x) const override {
                const auto eps2 = .5*eps;

                const auto r = x.norm();
                const auto t = radius;
                const auto z = Z(r,t);

                return z>-eps2 && z<+eps2;
            }

            Real operator()(Real2D x) const override {
                const auto r = x.norm();
                const auto t = radius;

                const auto z = Z(r,t);

                const auto d = delta(z);

                return d; //abs(z)>eps ? d : d + delta(-z);
            }
        };



        extern SpecialRingDelta *ringDelta1;
        extern Real ring_tf;



        class OutGL : public R2toR::OutputOpenGL {
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
            auto notifyKeyboard(unsigned char key, int x, int y) -> void override;

            void notifyMousePassiveMotion(int x, int y) override;

            void notifyMouseMotion(int x, int y) override;
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
                } else if (t<tf || tf<0){
                    const_cast<SpecialRingDelta&>(ringDelta).setRadius(t);

                    function.setDPhiDt(ringDelta);
                }
            }
        };

        class OutputBuilder : public OutputStructureBuilderR2toR {
        protected:
            auto buildOpenGLOutput() -> R2toR::OutputOpenGL * override {
                const double phiMin = -.2;
                const double phiMax =  .8;

                const auto &p = Numerics::Allocator::getInstance().getNumericParams();
                const Real L = p.getL();
                const Real xLeft = p.getxLeft();
                const Real xRight = xLeft + L;
                const auto dx = 0; // L*2.5/10;

                return new OutGL(xLeft-dx, xRight+dx, xLeft-dx, xRight+dx, phiMin, phiMax);
            }
        };

        class Input : public R2toRBCInterface {
            DoubleParameter a = DoubleParameter(0.1, "a", "The height of regularized delta;");
            DoubleParameter eps = DoubleParameter(0.1, "eps", "Half the base width of regularized delta;");
            DoubleParameter deltaDuration = DoubleParameter(-1, "delta_duration", "The duration of "
                                                                                  "regularized delta. Negative "
                                                                                  "values mean forever;");

        public:
            Input() : R2toRBCInterface("(2+1)-d Shockwave as trail of a driving delta.",
                                       "ldd", new OutputBuilder) { addParameters({a, eps, deltaDuration}); }

            auto getBoundary() const -> const void * override {
                auto &p = const_cast<NumericParams&>(Numerics::Allocator::getInstance().getNumericParams());
                const Real L = p.getL();

                p.sett(L*.5 - *eps *2);

                SpecialRingDelta *delta = nullptr;

                if (0) delta = new AzimuthDelta(*eps, *a);
                else   delta = new ZDelta(*eps, *a);

                return new BoundaryCondition(*delta, *deltaDuration);
            }
        };
    }
}


#endif //STUDIOSLAB_LEADINGDELTA_H
