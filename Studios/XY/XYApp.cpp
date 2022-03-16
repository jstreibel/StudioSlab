//
// Created by joao on 11/4/21.
//

#include "XYApp.h"

#include <Studios/Backend/SFML-Nuklear/SFML-Nuklear-Backend.h>

#include <Phys/Thermal/IO/SingleSim/SingleSimViewController.h>
#include <Phys/Thermal/Metropolis/MetropolisAlgorithm.h>
#include <Studios/Backend/DummyProgram.h>

XYApp::XYApp(int argc, const char **argv) : AppBase(argc, argv) {
    addParameters({&L, &T, &MCSteps, &TSteps, &transient});

    SFMLNuklearBackend::GetInstance();

    AppBase::parseCLArgs();
}


int XYApp::run() {

    if(0)
    {
        auto *viewControl = new ThermoOutput::SingleSimViewController(*L, *MCSteps, *transient);

        MetropolisAlgorithm mcCalculator(*L, *T, .0, viewControl,
                                         MetropolisAlgorithm::Ferromagnetic,
                                         MetropolisAlgorithm::Metropolis,
                                         MetropolisAlgorithm::Random);

        mcCalculator.Simulate(*MCSteps, *transient);
    }

    SFMLNuklearBackend::GetInstance()->run(new DummyProgram);

    return 0;
}