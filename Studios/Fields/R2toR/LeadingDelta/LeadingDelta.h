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
        class SpecialRingDelta : public FunctionAzimuthalSymmetry {
            const Real eps, height;

            Real radius;

        public:
            SpecialRingDelta(Real eps, Real height)
                    : FunctionAzimuthalSymmetry(new RtoR::RegularDiracDelta(eps, height)), eps(eps), height(height),
                      radius(0) { }

            bool domainContainsPoint(Real2D x) const override {
                const auto eps2 = .5*eps;

                const auto r = x.norm();

                return r>radius-eps2 && r<radius+eps2;
            }

            void setRadius(Real _radius) {
                radius = _radius;

                const auto &func = static_cast<const RtoR::RegularDiracDelta&>(this->getBaseFunction());
                auto &delta = const_cast<RtoR::RegularDiracDelta&>(func);
                delta.setTranslation(radius);
            }

            Real getRadius() const { return radius; };


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
            Real height;
            Real eps;

            SpecialRingDelta ringDelta;

        public:
            BoundaryCondition(Real eps = 0.1, Real height = 2 / 3.) : ringDelta(eps, height) {
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

                const Real xLeft = Numerics::Allocator::getInstance().getNumericParams().getxLeft();
                const Real xRight = xLeft + Numerics::Allocator::getInstance().getNumericParams().getL();

                return new OutGL(xLeft, xRight, xLeft, xRight, phiMin, phiMax);
            }
        };

        class Input : public R2toRBCInterface {
            DoubleParameter a = DoubleParameter(0.1, "a", "The height of regularized delta;");
            DoubleParameter eps = DoubleParameter(0.1, "eps", "The width of regularized delta;");

        public:
            Input() : R2toRBCInterface("(2+1)-d Shockwave as trail of a driving delta.",
                                       "ldd", new OutputBuilder) { addParameters({&a, &eps}); }

            auto getBoundary() const -> const void * override { return new BoundaryCondition(*eps, *a); }
        };
    }
}


#endif //STUDIOSLAB_LEADINGDELTA_H
