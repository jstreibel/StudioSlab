//
// Created by joao on 7/09/23.
//

#include "BoundaryCondition.h"

#include <utility>
#include "Math/Function/RtoR/Model/FunctionsCollection/Trigonometric.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Shockwave/SGPlaneWave.h"

namespace Modes {

    FPlaneWaveBC::FPlaneWaveBC(const KGRtoR::EquationState_constptr& prototype, Real Q, Real k)
    : BoundaryCondition(prototype, New <RtoR::NullFunction> (), New <RtoR::NullFunction> ())
    , Q(Q), k(k)
    {
    }

    void FPlaneWaveBC::ApplyKG(KGRtoR::EquationState& KGState, Real t) const
    {
        if(t==0.0)
        {
            fix Phi = RtoR::FSignumGordonPlaneWave(Q, k);
            fix PhiTimeDerivative = RtoR::FSignumGordonPlaneWave_TimeDerivative(Q, k);

            KGState.SetPhi(Phi);
            KGState.SetDPhiDt(PhiTimeDerivative);
        }
    }

    SignalBC::SignalBC(const KGRtoR::EquationState_ptr &prototype, const Real A, const Real ω)
    : BoundaryCondition(prototype, New <RtoR::NullFunction> (), New <RtoR::NullFunction> ())
    , A(A)
    , ω(ω) {}

    void SignalBC::ApplyKG(KGRtoR::EquationState &kgState, const Real t) const {
        if(t==0.0) BoundaryCondition::ApplyKG(kgState, t);

        OUT ϕ   = kgState.getPhi();
        OUT 𝜕ₜϕ = kgState.getDPhiDt() ;

        ϕ.getSpace().getHostData()[0] = A*sin(ω*t);
        𝜕ₜϕ.getSpace().getHostData()[0] = A*ω*cos(ω*t);
    }

    DrivenBC::DrivenBC(const KGRtoR::EquationState_ptr &prototype, Pointer<SquareWave> sqrWave)
    : BoundaryCondition(prototype, New <RtoR::NullFunction> (), New <RtoR::NullFunction> ()),
      sqrWave(std::move(sqrWave)) { }

    void DrivenBC::ApplyKG(KGRtoR::EquationState &toFunction, Real t) const {
        if(sqrWave != nullptr) sqrWave->set_t(t);

        BoundaryCondition::Apply(toFunction, t);
    }
}
