//
// Created by joao on 7/09/23.
//

#include "BoundaryCondition.h"

#include <utility>
#include "Math/Function/RtoR/Model/FunctionsCollection/Trigonometric.h"

Modes::SignalBC::SignalBC(const KGRtoR::EquationState_ptr &prototype, Real A, Real ω)
: BoundaryCondition(prototype, New <RtoR::NullFunction> (), New <RtoR::NullFunction> ())
, A(A)
, ω(ω) {}

void Modes::SignalBC::applyKG(Slab::Models::KGRtoR::EquationState &kgState, Slab::Real t) const {
    if(t==0.0) KGRtoR::BoundaryCondition::applyKG(kgState, t);

    OUT ϕ   = kgState.getPhi();
    OUT 𝜕ₜϕ = kgState.getDPhiDt() ;

    ϕ.getSpace().getHostData()[0] = A*sin(ω*t);
    𝜕ₜϕ.getSpace().getHostData()[0] = A*ω*cos(ω*t);
}

Modes::DrivenBC::DrivenBC(const KGRtoR::EquationState_ptr &prototype, Slab::Pointer<Modes::SquareWave> sqrWave)
: KGRtoR::BoundaryCondition(prototype, New <RtoR::NullFunction> (), New <RtoR::NullFunction> ()),
  sqrWave(std::move(sqrWave)) { }

void Modes::DrivenBC::applyKG(KGRtoR::EquationState &toFunction, Real t) const {
    if(sqrWave != nullptr) sqrWave->set_t(t);

    BoundaryCondition::apply(toFunction, t);
}
