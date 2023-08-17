//
// Created by joao on 8/4/23.
//

#include "ThermalBuilder.h"
#include "ThermalMonitor.h"
#include "Models/LangevinKleinGordon/LangevinKGSolver.h"
#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"

#define DONT_SELF_REGISTER false
#define SHORT_NAME false

namespace RtoR::Thermal {

    Builder::Builder(const Str &name, const Str &generalDescription, bool doRegister) : KGBuilder(name, generalDescription, DONT_SELF_REGISTER) {
        interface->addParameters({&temperature, &dissipation, &transientGuess});

        if(doRegister) registerToManager();
    }

    auto Builder::buildEquationSolver() -> void * {
        GET bc = *(RtoR::BoundaryCondition*)getBoundary();

        auto solver = new RtoR::LangevinKGSolver(numericParams, bc);
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

RtoR::Monitor *RtoR::Thermal::Builder::buildOpenGLOutput() {
    auto monitor = new RtoR::Thermal::Monitor(numericParams, *(KGEnergy*)getHamiltonian());
    monitor->setTransientGuess(*transientGuess);
    return monitor;
}
