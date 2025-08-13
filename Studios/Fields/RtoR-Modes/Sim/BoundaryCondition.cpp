//
// Created by joao on 7/09/23.
//

#include "BoundaryCondition.h"

#include <utility>
#include "Math/Function/RtoR/Model/FunctionsCollection/Trigonometric.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Shockwave/SGPlaneWave.h"

namespace Modes {
    FPlaneWaveBC::FPlaneWaveBC(const KGRtoR::EquationState_constptr& prototype, DevFloat Q, DevFloat k)
    : BoundaryCondition(prototype, New <RtoR::NullFunction> (), New <RtoR::NullFunction> ())
    , Q(Q), k(k)
    {
    }

    void FPlaneWaveBC::ApplyKG(KGRtoR::EquationState& KGState, DevFloat t) const
    {
        if(t==0.0)
        {
            fix Phi = RtoR::FSignumGordonPlaneWave(Q, k);
            fix PhiTimeDerivative = RtoR::FSignumGordonPlaneWave_TimeDerivative(Q, k);

            KGState.SetPhi(Phi);
            KGState.SetDPhiDt(PhiTimeDerivative);
        }
    }



    SignalBC::SignalBC(const KGRtoR::EquationState_ptr &prototype, const DevFloat A, const DevFloat ω)
    : BoundaryCondition(prototype, New <RtoR::NullFunction> (), New <RtoR::NullFunction> ())
    , A(A)
    , ω(ω) {}

    void SignalBC::ApplyKG(KGRtoR::EquationState &kgState, const DevFloat t) const {
        if(t==0.0) BoundaryCondition::ApplyKG(kgState, t);

        OUT ϕ   = kgState.getPhi();
        OUT 𝜕ₜϕ = kgState.getDPhiDt() ;

        ϕ.getSpace().getHostData()[0] = A*sin(ω*t);
        𝜕ₜϕ.getSpace().getHostData()[0] = A*ω*cos(ω*t);
    }

    DrivenBC::DrivenBC(const KGRtoR::EquationState_ptr &prototype, TPointer<SquareWave> sqrWave)
    : BoundaryCondition(prototype, New <RtoR::NullFunction> (), New <RtoR::NullFunction> ()),
      sqrWave(std::move(sqrWave)) { }

    void DrivenBC::ApplyKG(KGRtoR::EquationState &toFunction, DevFloat t) const {
        if(sqrWave != nullptr) sqrWave->set_t(t);

        BoundaryCondition::Apply(toFunction, t);
    }
}
