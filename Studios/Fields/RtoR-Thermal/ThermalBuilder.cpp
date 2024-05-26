//
// Created by joao on 8/4/23.
//

#include "ThermalBuilder.h"

#include <memory>
#include "Models/LangevinKleinGordon/LangevinKGSolver.h"
#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"
#include "Models/KleinGordon/RtoR/Graphics/RtoRStatisticalMonitor.h"
#include "../RtoR-Modes/Sim/Monitor.h"


#define DONT_SELF_REGISTER false
#define SHORT_NAME false

namespace Studios::Fields::RtoRThermal {

    Builder::Builder(const Str &name, const Str &generalDescription, bool doRegister)
    : KGBuilder(name, generalDescription, DONT_SELF_REGISTER) {
        interface->addParameters({&temperature, &dissipation, &transientGuess});

        setLaplacianPeriodicBC();

        if(doRegister) registerToManager();
    }

    auto Builder::buildEquationSolver() -> void * {
        GET bc = *(Slab::Models::KGRtoR::BoundaryCondition*)getBoundary();

        auto solver = new Slab::Models::KGRtoR::LangevinKGSolver(simulationConfig.numericConfig, bc);
        solver->setTemperature(*temperature);
        solver->setDissipationCoefficient(*dissipation);

        return solver;
    }

    Str Builder::suggestFileName() const {
        auto str = VoidBuilder::suggestFileName();

        auto extra = interface->toString({"T", "k"}, " ", SHORT_NAME);

        return str + " " + extra;
    }


}
