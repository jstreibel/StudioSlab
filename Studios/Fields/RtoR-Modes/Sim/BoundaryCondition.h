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


    class FPlaneWaveBC final : public KGRtoR::FBoundaryCondition {
        DevFloat Q, k;
    public:
        explicit FPlaneWaveBC(const KGRtoR::FEquationState_constptr& prototype, DevFloat Q, DevFloat k);

    protected:
        void ApplyKG(KGRtoR::FEquationState& KGState, DevFloat t) const override;
    };


    class FSignalBC final : public KGRtoR::FBoundaryCondition {
    protected:
        void ApplyKG(KGRtoR::FEquationState &, DevFloat t) const override;

    public:
        DevFloat A, ω;

        FSignalBC(const KGRtoR::FEquationState_ptr &prototype, DevFloat A, DevFloat ω);
    };


    class FDrivenBC final : public KGRtoR::FBoundaryCondition {
        Slab::TPointer<Modes::FSquareWave> sqrWave;

    protected:
        void ApplyKG(KGRtoR::FEquationState &, DevFloat t) const override;

    public:
        FDrivenBC(const KGRtoR::FEquationState_ptr &prototype, Slab::TPointer<Modes::FSquareWave> sqrWave);
    };

    using SignalBC [[deprecated("Use FSignalBC")]] = FSignalBC;
    using DrivenBC [[deprecated("Use FDrivenBC")]] = FDrivenBC;

} // Modes

#endif //STUDIOSLAB_BOUNDARYCONDITION_H
