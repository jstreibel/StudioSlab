//
// Created by joao on 8/4/23.
//

#include "ThermalBuilder.h"
#include "Models/LangevinKleinGordon/LangevinKGSolver.h"
#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"

#define DONT_SELF_REGISTER false

namespace RtoR::Thermal {

    Builder::Builder(const Str &name, const Str &generalDescription, bool doRegister) : KGBuilder(name, generalDescription, DONT_SELF_REGISTER) {
        interface->addParameters({&temperature, &dissipation});

        if(doRegister) registerToManager();
    }

    auto Builder::buildEquationSolver() -> void * {
        GET bc = *(RtoR::BoundaryCondition*)getBoundary();

        auto solver = new RtoR::LangevinKGSolver(numericParams, bc);
        solver->setTemperature(*temperature);
        solver->setDissipationCoefficient(*dissipation);

        return solver;
    }
}