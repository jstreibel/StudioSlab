//
// Created by joao on 12/24/22.
//

#ifndef STUDIOSLAB_LEADINGDELTAINPUT_H
#define STUDIOSLAB_LEADINGDELTAINPUT_H

#include <Fields/Mappings/R2toR/Controller/R2ToRBCInterface.h>

#include "../OutputBuilder.h"
#include "LeadingDeltaBoundaryConditoin.h"
#include "Fields/Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Fields/Mappings/R2toR/Model/FunctionsCollection/R2ToRRegularDelta.h"
#include "Fields/Mappings/R2toR/Model/BoundaryConditions/R2ToRBoundaryCondition.h"


namespace R2toR {

    class LeadingDeltaInput : public R2toRBCInterface {
    public:
        LeadingDeltaInput() : R2toRBCInterface("(2+1)-d leading Dirac delta shockwave", "ldd",
                                             new OutputBuilder) { }

        auto getBoundary() const -> const void * override{
            return new LeadingDeltaBoundaryCondition;
        }
    };
}


#endif //STUDIOSLAB_LEADINGDELTAINPUT_H
