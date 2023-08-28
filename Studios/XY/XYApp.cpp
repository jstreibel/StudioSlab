//
// Created by joao on 11/4/21.
//

#include "XYApp.h"
#include "Core/Backend/GUIBackend.h"

#include "Core/Backend/SFML-Nuklear/SFML-Nuklear-Backend.h"

#include <Math/Thermal/IO/SingleSim/SingleSimViewController.h>
#include <Math/Thermal/Metropolis/MetropolisAlgorithm.h>

#define DONT_SELF_REGISTER false

XY::App::App(int argc, const char **argv) : AppBase(argc, argv, DONT_SELF_REGISTER) {
    getInterface()->addParameters({&N, &T, &MCSteps, &transient});

    registerToManager();

    AppBase::parseCLArgs();
}


int XY::App::App::run() {

    if(1)
    {
        auto *viewControl = new ThermoOutput::SingleSimViewController(*N, *MCSteps, *transient);

        MetropolisAlgorithm mcCalculator(*N, *T, .0, viewControl,
                                         MetropolisAlgorithm::Ferromagnetic,
                                         MetropolisAlgorithm::Metropolis,
                                         MetropolisAlgorithm::Random);

        mcCalculator.Simulate(*MCSteps, *transient);
    }

    return 0;
}