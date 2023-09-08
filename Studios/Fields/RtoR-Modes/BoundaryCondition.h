//
// Created by joao on 7/09/23.
//

#ifndef STUDIOSLAB_BOUNDARYCONDITION_H
#define STUDIOSLAB_BOUNDARYCONDITION_H

#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"
#include "Maps/RtoR/Model/FunctionsCollection/NullFunction.h"

namespace Modes {

    class SignalBC : public RtoR::BoundaryCondition {
    public:
        Real A, ω;

        SignalBC(const RtoR::EquationState &prototype, Real A, Real ω);

        void apply(EqState &toFunction, Real t) const override;
    };

} // Modes

#endif //STUDIOSLAB_BOUNDARYCONDITION_H
