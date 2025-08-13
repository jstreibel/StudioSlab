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


    class FPlaneWaveBC final : public KGRtoR::BoundaryCondition {
        DevFloat Q, k;
    public:
        explicit FPlaneWaveBC(const KGRtoR::EquationState_constptr& prototype, DevFloat Q, Real k);

    protected:
        void ApplyKG(KGRtoR::EquationState& KGState, DevFloat t) const override;
    };


    class SignalBC final : public KGRtoR::BoundaryCondition {
    protected:
        void ApplyKG(KGRtoR::EquationState &, DevFloat t) const override;

    public:
        DevFloat A, ω;

        SignalBC(const KGRtoR::EquationState_ptr &prototype, DevFloat A, DevFloat ω);
    };


    class DrivenBC final : public KGRtoR::BoundaryCondition {
        Slab::TPointer<Modes::SquareWave> sqrWave;

    protected:
        void ApplyKG(KGRtoR::EquationState &, DevFloat t) const override;

    public:
        DrivenBC(const KGRtoR::EquationState_ptr &prototype, Slab::TPointer<Modes::SquareWave> sqrWave);
    };


} // Modes

#endif //STUDIOSLAB_BOUNDARYCONDITION_H
