//
// Created by joao on 7/09/23.
//

#include "BoundaryCondition.h"

#include <utility>
#include "Math/Function/Maps/RtoR/Model/FunctionsCollection/Trigonometric.h"

Modes::SignalBC::SignalBC(const RtoR::EquationState &prototype, Real A, Real ω)
: BoundaryCondition(prototype, new RtoR::NullFunction(), new RtoR::NullFunction())
, A(A)
, ω(ω) {}

void Modes::SignalBC::apply(EqState &toFunction, Real t) const {
    if(t==0.0) RtoR::BoundaryCondition::apply(toFunction, t);

    OUT ϕ = toFunction.getPhi();
    OUT 𝜕ₜϕ = toFunction.getDPhiDt() ;

    ϕ.getSpace().getHostData()[0] = A*sin(ω*t);
    𝜕ₜϕ.getSpace().getHostData()[0] = A*ω*cos(ω*t);
}


Modes::DrivenBC::DrivenBC(const RtoR::EquationState &prototype, std::shared_ptr<Modes::SquareWave> sqrWave)
: BoundaryCondition(prototype, new RtoR::NullFunction, new RtoR::NullFunction),
  sqrWave(std::move(sqrWave)) { }

void Modes::DrivenBC::apply(RtoR::EquationState &toFunction, Real t) const {
    if(sqrWave != nullptr) sqrWave->set_t(t);

    BoundaryCondition::apply(toFunction, t);
}
