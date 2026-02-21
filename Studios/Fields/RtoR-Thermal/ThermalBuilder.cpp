//
// Created by joao on 8/4/23.
//

#include "ThermalBuilder.h"

#include <memory>
#include "Models/KleinGordon/KG-LangevinSolver.h"
#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoRBoundaryCondition.h"
#include "Models/KleinGordon/RtoR/Graphics/Panels/RtoRStatisticalMonitor.h"
#include "../RtoR-Modes/Sim/Monitor.h"

// Don't touch:
#define DONT_SELF_REGISTER false
#define SHORT_NAME false
#define LONG_NAME true

namespace Studios::Fields::RtoRThermal {

    FBuilder::FBuilder(const Str &name, const Str &generalDescription, bool doRegister)
    : FKGRtoR_Recipe(name, generalDescription, DONT_SELF_REGISTER) {
        Interface->AddParameters({&temperature, &dissipation, &transientGuess});

        SetLaplacianPeriodicBC();

        if(doRegister) RegisterToManager();
    }

    auto FBuilder::buildSolver() -> TPointer<Base::LinearStepSolver> {
        if(!(*temperature>0)) {
            Log::Info() << "Temperature is set to ZERO. Using usual non-stochastic Klein-Gordon solver." << Log::Flush;
            return KGRtoR::FKGRtoR_Recipe::buildSolver();
        }

        auto ϕ_at_𝜕Ω = GetBoundary();
        auto V = getPotential();

        using Solver = Slab::Models::KGRtoR::KGLangevinSolver;
        auto solver = New<Solver>(ϕ_at_𝜕Ω, V);

        solver->setTemperature(*temperature);
        solver->setDissipationCoefficient(*dissipation);

        return solver;
    }

    Str FBuilder::SuggestFileName() const {
        auto str = FNumericalRecipe::SuggestFileName();

        auto extra1 = Interface->ToString({"T", "k"}, " ", SHORT_NAME);
        auto extra2 = Interface->ToString({"E", "n"}, " ", LONG_NAME);

        return str + " " + extra1 + " " + extra2;
    }


}
