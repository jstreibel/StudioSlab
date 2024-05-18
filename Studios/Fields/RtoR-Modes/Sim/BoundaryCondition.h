//
// Created by joao on 7/09/23.
//

#ifndef STUDIOSLAB_BOUNDARYCONDITION_H
#define STUDIOSLAB_BOUNDARYCONDITION_H

#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "SquareWave.h"

namespace Modes {

    class SignalBC : public RtoR::BoundaryCondition {
    public:
        Real A, ω;

        SignalBC(const RtoR::EquationState &prototype, Real A, Real ω);

        void apply(EqState &toFunction, Real t) const override;
    };

    class DrivenBC : public RtoR::BoundaryCondition {
        std::shared_ptr<Modes::SquareWave> sqrWave;
    public:

        DrivenBC(const RtoR::EquationState &prototype, std::shared_ptr<Modes::SquareWave> sqrWave);

        void apply(EqState &toFunction, Real t) const override;
    };

} // Modes

#endif //STUDIOSLAB_BOUNDARYCONDITION_H
