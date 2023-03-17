//
// Created by joao on 11/15/22.
//

#ifndef STUDIOSLAB_DIRACSPEEDINPUT_H
#define STUDIOSLAB_DIRACSPEEDINPUT_H

#include <Fields/Mappings/R2toR/Controller/R2ToRBCInterface.h>

#include "GrowingHole/OutputBuilder.h"
#include "GrowingHole/GrowingHoleBoundaryCondition.h"
#include "Fields/Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Fields/Mappings/R2toR/Model/FunctionsCollection/R2ToRRegularDelta.h"
#include "Fields/Mappings/R2toR/Model/BoundaryConditions/R2ToRBoundaryCondition.h"


namespace R2toR {
    class InputDiracSpeed : public R2toRBCInterface {
        DoubleParameter eps   =DoubleParameter{1., "eps", "Quasi-shockwave 'epsilon' parameter."};
        DoubleParameter E     =DoubleParameter{1., "E", "Total energy."};

    public:
        InputDiracSpeed() : R2toRBCInterface("(2+1)-d Shockwave as a growing hole.", "gh",
                                              new OutputBuilder) { addParameters({&E, &eps}); }

        auto getBoundary() const -> const void * override{
            auto E = *this->E;
            auto eps = *this->eps;
            const double a = sqrt((4./3)*pi*eps*eps*E);

            let *phi0 = new FunctionAzimuthalSymmetry(new RtoR::NullFunction);
            let *dPhiDt0 = new R2toR::R2toRRegularDelta(eps, a);

            return new BoundaryCondition(phi0, dPhiDt0);
        }
    };
}

#endif //STUDIOSLAB_DIRACSPEEDINPUT_H
