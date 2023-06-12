//
// Created by joao on 08/06/23.
//

#include "Mappings/RtoR/Model/FunctionsCollection/AbsFunction.h"

#include "DrivenEquation.h"
#include "Phys/Numerics/Allocator.h"
#include "Common/Log/Log.h"

namespace R2toR {

    namespace LeadingDelta {


        DrivenEquation::DrivenEquation(R2toR::Function::Ptr drivingForce)
            : GordonSystem(*(new RtoR::AbsFunction)), drivingForce(drivingForce)
        {
            if(drivingForce == nullptr) {
                Log::Error() << "DrivenEquation solver's driving force must be != nullptr." << Log::Flush;
                throw "Error allocating driven equation solver.";
            }
            Log::Info() << "Driven equation solver allocated. Driving force '" << drivingForce->myName() << "'." << Log::Flush;
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

                auto &f = *drivingForce;

                auto &dVdϕ = *dVDPhi;
                auto &dVdϕₒᵤₜ = dV_out;

                ϕᵢₙ.Laplacian(laplacian);
                ϕᵢₙ.Apply(dVdϕ, dVdϕₒᵤₜ);

                𝜕φ𝜕tₒᵤₜ.StoreSubtraction(laplacian, dVdϕₒᵤₜ);

                (𝜕φ𝜕tₒᵤₜ+=f) *= δt;
            }

            return out;
        }

    }

}