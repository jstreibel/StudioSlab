//
// Created by joao on 7/09/23.
//

#ifndef STUDIOSLAB_BOUNDARYCONDITION_H
#define STUDIOSLAB_BOUNDARYCONDITION_H

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoRBoundaryCondition.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "SquareWave.h"

namespace Modes {

    using namespace Slab::Models;



    class SignalBC final : public KGRtoR::BoundaryCondition {
    protected:
        void applyKG(KGRtoR::EquationState &, Real t) const override;

    public:
        Real A, ω;

        SignalBC(const KGRtoR::EquationState_ptr &prototype, Real A, Real ω);
    };



    class DrivenBC final : public KGRtoR::BoundaryCondition {
        Slab::Pointer<Modes::SquareWave> sqrWave;

    protected:
        void applyKG(KGRtoR::EquationState &, Real t) const override;

    public:
        DrivenBC(const KGRtoR::EquationState_ptr &prototype, Slab::Pointer<Modes::SquareWave> sqrWave);
    };

} // Modes

#endif //STUDIOSLAB_BOUNDARYCONDITION_H
