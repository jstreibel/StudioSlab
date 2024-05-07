//
// Created by joao on 8/4/23.
//

#include "ThermalBuilder.h"

#include <memory>
// #include "ThermalMonitor.h"
#include "Models/LangevinKleinGordon/LangevinKGSolver.h"
#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"
#include "Graphics/Graph/ℝ↦ℝ/RtoRStatisticalMonitor.h"
#include "Graphics/Graph/ℝ↦ℝ/CorrelationsPanel.h"

#define DONT_SELF_REGISTER false
#define SHORT_NAME false

namespace RtoR::Thermal {

    Builder::Builder(const Str &name, const Str &generalDescription, bool doRegister)
    : KGBuilder(name, generalDescription, DONT_SELF_REGISTER) {
        interface->addParameters({&temperature, &dissipation, &transientGuess});

        setLaplacianPeriodicBC();

        if(doRegister) registerToManager();
    }

    auto Builder::buildEquationSolver() -> void * {
        GET bc = *(RtoR::BoundaryCondition*)getBoundary();

        auto solver = new RtoR::LangevinKGSolver(simulationConfig.numericConfig, bc);
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
    auto monitor = KGBuilder::buildOpenGLOutput();
    auto &guiWindow = monitor->getGUIWindow();
    auto &config = simulationConfig.numericConfig;
    auto &hamiltonian = *(KGEnergy*)getHamiltonian();



    auto temp1 = std::make_shared<RtoR::StatisticalMonitor>(
            config, hamiltonian, guiWindow);
    temp1->setTransientHint(*transientGuess);
    auto temp2 = std::make_shared<Graphics::CorrelationsPanel>(
            config, guiWindow, hamiltonian);

    monitor->addDataView(temp1);
    monitor->addDataView(temp2);

    return monitor;
}
