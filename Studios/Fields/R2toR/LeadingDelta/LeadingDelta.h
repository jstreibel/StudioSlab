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

                const auto &func = static_cast<const RtoR::RegularDiracDelta&>(this->getBaseFunction());
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
            ZDelta(Real eps, Real height) : SpecialRingDelta(eps, height), delta(eps, height) { }

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

                return delta(z);
            }
        };



        extern SpecialRingDelta *ringDelta1;



        class OutGL : public R2toR::OutputOpenGL {
        public:
            OutGL(Real xMin, Real xMax, Real yMin, Real yMax, Real phiMin, Real phiMax)
                    : R2toR::OutputOpenGL(xMin, xMax, yMin, yMax, phiMin, phiMax) {}

            void draw() override;

            IntPair getWindowSizeHint() override;
        };



        class BoundaryCondition : public Base::BoundaryConditions<R2toR::FieldState> {
            SpecialRingDelta &ringDelta;

        public:
            explicit BoundaryCondition(SpecialRingDelta &ringDelta) : ringDelta(ringDelta) {
                ringDelta1 = &ringDelta;
            }


            void apply(FieldState &function, Real t) const override {
                if (t == 0) {
                    RtoR::NullFunction nullFunction;
                    R2toR::FunctionAzimuthalSymmetry fullNull(&nullFunction, 1, 0, 0, false);

                    function.setPhi(fullNull);
                    function.setDPhiDt(fullNull);
                }

                const_cast<SpecialRingDelta&>(ringDelta).setRadius(t);

                function.setDPhiDt(ringDelta);
            }
        };

        class OutputBuilder : public OutputStructureBuilderR2toR {
        protected:
            auto buildOpenGLOutput() -> R2toR::OutputOpenGL * override {
                const double phiMin = -.5;
                const double phiMax = 1.;

                const auto &p = Numerics::Allocator::getInstance().getNumericParams();
                const Real L = p.getL();
                const Real xLeft = p.getxLeft();
                const Real xRight = xLeft + L;
                const auto dx = L*2.5/10;

                return new OutGL(xLeft-dx, xRight+dx, xLeft-dx, xRight+dx, phiMin, phiMax);
            }
        };

        class Input : public R2toRBCInterface {
            DoubleParameter a = DoubleParameter(0.1, "a", "The height of regularized delta;");
            DoubleParameter eps = DoubleParameter(0.1, "eps", "Half the base width of regularized delta;");

        public:
            Input() : R2toRBCInterface("(2+1)-d Shockwave as trail of a driving delta.",
                                       "ldd", new OutputBuilder) { addParameters({&a, &eps}); }

            auto getBoundary() const -> const void * override {
                SpecialRingDelta *delta = nullptr;

                if (0) delta = new AzimuthDelta(*eps, *a);
                else   delta = new ZDelta(*eps, *a);

                return new BoundaryCondition(*delta);
            }
        };
    }
}


#endif //STUDIOSLAB_LEADINGDELTA_H
