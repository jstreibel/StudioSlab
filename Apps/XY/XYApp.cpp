//
// Created by joao on 11/4/21.
//

#include "XYApp.h"

#include <Phys/Thermal/IO/SingleSim/SingleSimViewController.h>
#include <Phys/Thermal/Metropolis/MetropolisAlgorithm.h>

XYApp::XYApp(int argc, const char **argv) : AppBase(argc, argv) {
    addParameters({&L, &T, &MCSteps, &TSteps, &transient});

    AppBase::parseCLArgs();
}


int XYApp::run() {

    auto *viewControl = new ThermoOutput::SingleSimViewController(*L, *MCSteps, *transient);

    MetropolisAlgorithm mcCalculator(*L, *T, .0, viewControl,
                                     MetropolisAlgorithm::Ferromagnetic,
                                     MetropolisAlgorithm::Metropolis,
                                     MetropolisAlgorithm::Random);

    mcCalculator.Simulate(*MCSteps, *transient);

    return 0;
}