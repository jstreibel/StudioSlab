//
// Created by joao on 7/09/23.
//

#ifndef STUDIOSLAB_BOUNDARYCONDITION_H
#define STUDIOSLAB_BOUNDARYCONDITION_H

#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "SquareWave.h"

namespace Modes {

    using namespace Slab::Models;

    class SignalBC : public KGRtoR::BoundaryCondition {
    public:
        Real A, ω;

        SignalBC(const KGRtoR::EquationState &prototype, Real A, Real ω);

        void apply(EqState &toFunction, Real t) const override;
    };

    class DrivenBC : public KGRtoR::BoundaryCondition {
        Slab::Pointer<Modes::SquareWave> sqrWave;
    public:

        DrivenBC(const KGRtoR::EquationState &prototype, Slab::Pointer<Modes::SquareWave> sqrWave);

        void apply(EqState &toFunction, Real t) const override;
    };

} // Modes

#endif //STUDIOSLAB_BOUNDARYCONDITION_H
