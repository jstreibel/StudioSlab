//
// Created by joao on 08/06/23.
//

#include "Math/Function/RtoR/Model/FunctionsCollection/AbsFunction.h"

#include "DrivenEquation.h"
#include "Core/Tools/Log.h"

namespace Slab::Math::R2toR {

    namespace LeadingDelta {


        DrivenEquation::DrivenEquation(const NumericParams &params, R2toR::Function::Ptr drivingForce)
            : GordonSolverT(params, *(new RtoR::AbsFunction))
            , drivingForce(drivingForce)
            , drivingForceRendered(builder.getNumericParams().getN(),
                                   builder.getNumericParams().getxLeft(),
                                   builder.getNumericParams().geth())
        {
            if(drivingForce == nullptr) {
                Log::Error() << "DrivenEquation solver's driving force must be != nullptr." << Log::Flush;
                throw "Error allocating driven equation solver.";
            }
            Log::Note() << "Driven equation solver allocated. Driving force '"
                                    << drivingForce->myName() << "'." << Log::Flush;
        }

        auto DrivenEquation::dtF(const EquationState &in, EquationState &out, Real t, Real δt) -> EquationState & {
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

                drivingForce->renderToDiscreteFunction(&drivingForceRendered);
                auto &δ = drivingForceRendered;
                auto &dVdϕ = *dVDPhi;
                auto &dVdϕₒᵤₜ = dV_out;

                ϕᵢₙ.Laplacian(laplacian);
                ϕᵢₙ.Apply(dVdϕ, dVdϕₒᵤₜ);

                𝜕φ𝜕tₒᵤₜ.StoreSubtraction(laplacian, dVdϕₒᵤₜ);
                𝜕φ𝜕tₒᵤₜ.Add(δ);

                𝜕φ𝜕tₒᵤₜ *= δt;
            }

            return out;
        }

    }

}