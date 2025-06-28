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

    Builder::Builder(const Str &name, const Str &generalDescription, bool doRegister)
    : KGRtoRBuilder(name, generalDescription, DONT_SELF_REGISTER) {
        Interface->AddParameters({&temperature, &dissipation, &transientGuess});

        setLaplacianPeriodicBC();

        if(doRegister) RegisterToManager();
    }

    auto Builder::buildSolver() -> Pointer<Base::LinearStepSolver> {
        if(!(*temperature>0)) {
            Log::Info() << "Temperature is set to ZERO. Using usual non-stochastic Klein-Gordon solver." << Log::Flush;
            return KGRtoR::KGRtoRBuilder::buildSolver();
        }

        auto ϕ_at_𝜕Ω = getBoundary();
        auto V = getPotential();

        using Solver = Slab::Models::KGRtoR::KGLangevinSolver;
        auto solver = New<Solver>(ϕ_at_𝜕Ω, V);

        solver->setTemperature(*temperature);
        solver->setDissipationCoefficient(*dissipation);

        return solver;
    }

    Str Builder::suggestFileName() const {
        auto str = NumericalRecipe::suggestFileName();

        auto extra1 = Interface->ToString({"T", "k"}, " ", SHORT_NAME);
        auto extra2 = Interface->ToString({"E", "n"}, " ", LONG_NAME);

        return str + " " + extra1 + " " + extra2;
    }


}
