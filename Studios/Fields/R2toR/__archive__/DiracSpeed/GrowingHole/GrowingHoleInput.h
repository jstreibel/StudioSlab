//
// Created by joao on 10/11/22.
//

#ifndef STUDIOSLAB_GROWINGHOLEINPUT_H
#define STUDIOSLAB_GROWINGHOLEINPUT_H

#include "Mappings/R2toR/Controller/R2ToRSimBuilder.h"

#include "GrowingHoleBoundaryCondition.h"
#include "GrowingHoleGLOutput.h"

const auto pi = 3.1415926535897932384626;

namespace R2toR {
    namespace GrowingHole {

        class Builder : public R2toR::Builder {
            RealParameter::Ptr height = RealParameter::New(2. / 3, "h,height", "Discontinuity value.");

        public:
            Builder() : R2toR::Builder("GH", "(2+1)-d Shockwave as a growing hole") {
                interface->addParameters({height});
            }

            auto getInitialState() -> void * override {
                const Real a = sqrt((4. / 3) * pi * (*eps) * (*eps) * (*E));

                let *phi0 = new FunctionAzimuthalSymmetry(new RtoR::NullFunction);
                let *dPhiDt0 = new R2toR::R2toRRegularDelta(*eps, a);

                auto &u_0 = *(R2toR::EquationState*)newFieldState();

                u_0.setPhi(*phi0);
                u_0.setDPhiDt(*dPhiDt0);

                delete phi0;
                delete dPhiDt0;

                return &u_0;
            }

            auto getBoundary() -> void * override {
                const Real a = **this->height; // Eh isso mesmo? Não era h = 2a/3??

                return new GrowingHoleBoundaryCondition(a);
            }

            auto buildOpenGLOutput() -> R2toR::OutputOpenGL * override {
                const Real phiMin = -.5;
                const Real phiMax = 1;

                return new R2toR::GrowingHoleOutGL(numericParams, phiMin, phiMax);
            }


        };
    }
}


#endif //STUDIOSLAB_GROWINGHOLEINPUT_H
