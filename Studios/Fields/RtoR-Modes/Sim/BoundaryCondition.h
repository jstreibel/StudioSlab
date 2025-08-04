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
        Real Q, k;
    public:
        explicit FPlaneWaveBC(const KGRtoR::EquationState_constptr& prototype, Real Q, Real k);

    protected:
        void ApplyKG(KGRtoR::EquationState& KGState, Real t) const override;
    };


    class SignalBC final : public KGRtoR::BoundaryCondition {
    protected:
        void ApplyKG(KGRtoR::EquationState &, Real t) const override;

    public:
        Real A, ω;

        SignalBC(const KGRtoR::EquationState_ptr &prototype, Real A, Real ω);
    };


    class DrivenBC final : public KGRtoR::BoundaryCondition {
        Pointer<SquareWave> sqrWave;

    protected:
        void ApplyKG(KGRtoR::EquationState &, Real t) const override;

    public:
        DrivenBC(const KGRtoR::EquationState_ptr &prototype, Pointer<SquareWave> sqrWave);
    };


} // Modes

#endif //STUDIOSLAB_BOUNDARYCONDITION_H
