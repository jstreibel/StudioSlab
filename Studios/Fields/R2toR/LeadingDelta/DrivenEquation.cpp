//
// Created by joao on 08/06/23.
//

#include "Mappings/RtoR/Model/FunctionsCollection/AbsFunction.h"

#include "DrivenEquation.h"
#include "Phys/Numerics/Allocator.h"

namespace R2toR {

    namespace LeadingDelta {


        DrivenEquation::DrivenEquation(Real eps, Real W_0, Real tMax)
            : GordonSystem(*(new RtoR::AbsFunction)),
              δᵣ(*(FunctionArbitrary*)Numerics::Allocator::getInstance().newFunctionArbitrary()),
              tMax(tMax)
        {
            std::cout << "Integrating driven equation" << std::endl;

            auto δt = Numerics::Allocator::getInstance().getNumericParams().getdt();;
            ringDelta = new Delta_r(eps, .5*W_0, δt);
        }

        auto DrivenEquation::dtF(const FieldState &in, FieldState &out, Real t, Real δt) -> FieldState & {
            // φ = dϕ/dt

            const
            auto & ϕᵢₙ     = in.getPhi();
            const
            auto & 𝜕φ𝜕tᵢₙ  = in.getDPhiDt();

            auto & ϕₒᵤₜ    = out.getPhi();
            auto & 𝜕φ𝜕tₒᵤₜ = out.getDPhiDt();

            // Eq 1
            {
                // φ = dφ/dt * dt
                ϕₒᵤₜ.SetArb(𝜕φ𝜕tᵢₙ) *= δt;
            }

            // Eq 2
            {
                // d²ϕ/dt² = dφ/dt = ∇ϕ - sign(ϕ) + ½W(0)δ(z)
                // ϕ=ϕ(t,r)
                // z=¼(r²-t²)
                // δ(z)𝕕z = r/t δ(r-t)𝕕r

                δᵣ = *ringDelta;

                ϕᵢₙ.Laplacian(laplacian);
                ϕᵢₙ.Apply(dVDPhi, dV_out);

                𝜕φ𝜕tₒᵤₜ.StoreSubtraction(laplacian, dV_out);

                𝜕φ𝜕tₒᵤₜ.Add(δᵣ) *= δt;
            }

            return out;
        }

    }

}