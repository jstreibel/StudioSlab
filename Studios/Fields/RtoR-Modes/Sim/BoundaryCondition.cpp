//
// Created by joao on 7/09/23.
//

#include "BoundaryCondition.h"
#include "Maps/RtoR/Model/FunctionsCollection/Trigonometric.h"

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