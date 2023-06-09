//
// Created by joao on 08/06/23.
//

#include "DrivenEquation.h"
#include "Mappings/RtoR/Model/FunctionsCollection/AbsFunction.h"

namespace R2toR {

    DrivenEquation::DrivenEquation() : GordonSystem(* (new RtoR::AbsFunction)) {}

    auto DrivenEquation::dtF(const FieldState &in, FieldState &out, Real t, Real dt) -> FieldState& {
        // φ = dϕ/dt

        const auto & ϕᵢₙ     = in.getPhi();
        const auto & 𝜕φ𝜕tᵢₙ  = in.getDPhiDt();
        auto       & ϕₒᵤₜ    = out.getPhi();
        auto       & 𝜕φ𝜕tₒᵤₜ = out.getDPhiDt();

        // Eq 1
        {
            // φ = dφ/dt * dt
            ϕₒᵤₜ    .SetArb(𝜕φ𝜕tᵢₙ) *= dt;
        }

        // Eq 2
        {
            // d²ϕ/dt² = dφ/dt = ∇ϕ - sign(ϕ) + ¹₂

            ϕᵢₙ     .Laplacian(laplacian);
            ϕᵢₙ     .Apply(dVDPhi, dV);


            𝜕φ𝜕tₒᵤₜ .StoreSubtraction(laplacian, dV) *= dt;
        }

        return out;
    }


    void *DrivenEqAllocator::getSystemSolver() {
        return new DrivenEquation();
    }
}