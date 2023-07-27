//
// Created by joao on 11/15/22.
//

#ifndef STUDIOSLAB_DIRACSPEEDINPUT_H
#define STUDIOSLAB_DIRACSPEEDINPUT_H

#include "Mappings/R2toR/Controller/R2ToRSimBuilder.h"

#include "../GrowingHole/GrowingHoleBoundaryCondition.h"
#include "../GrowingHole/GrowingHoleInput.h"

#include "Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Mappings/R2toR/Model/FunctionsCollection/R2ToRRegularDelta.h"
#include "Mappings/R2toR/Model/BoundaryConditions/R2ToRBoundaryCondition.h"


namespace R2toR {
    namespace DiracSpeed {
    class Builder : public R2toR::Builder {
            RealParameter::Ptr eps = RealParameter::New(1., "eps", "Quasi-shockwave 'epsilon' parameter.");
            RealParameter::Ptr E =   RealParameter::New(1., "E", "Total energy.");

        public:
            Builder() : R2toR::Builder("DiracSpeed",
                                          "(2+1)-d shockwave-like formation from an initial dirac-delta "
                                          "field time-derivative profile.") {
                interface->addParameters({E, eps});
            }

            auto getBoundary() -> void * override {
                const Real a = sqrt((4. / 3) * pi * (*eps) * (*eps) * (*E));

                let *phi0 = new FunctionAzimuthalSymmetry(new RtoR::NullFunction);
                let *dPhiDt0 = new R2toR::R2toRRegularDelta(*eps, a);

                return new BoundaryCondition(phi0, dPhiDt0);
            }

    protected:
        auto buildOpenGLOutput() -> R2toR::OutputOpenGL * override {
            const Real phiMin = -.5;
            const Real phiMax = 1;

            return new R2toR::GrowingHoleOutGL(numericParams, phiMin, phiMax);
        }
    };

    }
}

#endif //STUDIOSLAB_DIRACSPEEDINPUT_H
