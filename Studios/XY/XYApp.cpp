//
// Created by joao on 11/4/21.
//

#include "XYApp.h"
#include "Core/Backend/GraphicBackend.h"

#include "Core/Backend/SFML/SFMLBackend.h"
#include "Core/Backend/BackendManager.h"
#include "Core/Backend/Program/DummyProgram.h"

#include <Math/Thermal/IO/SingleSim/SingleSimViewController.h>
#include "Math/Thermal/MetropolisAlgorithm.h"

#define DONT_SELF_REGISTER false

XY::App::App(int argc, const char **argv) : AppBase(argc, argv, DONT_SELF_REGISTER) {
    getInterface()->addParameters({&N, &T, &MCSteps, &transient});

    registerToManager();

    Core::BackendManager::Startup(Core::SFML);
    Core::BackendManager::LoadModule(Core::Nuklear);

    AppBase::parseCLArgs();
}


int XY::App::App::run() {


    auto *viewControl = new ThermoOutput::SingleSimViewController(*N, *MCSteps, *transient);

    MetropolisAlgorithm mcCalculator(*N, *T, .0,
                                     MetropolisAlgorithm::Ferromagnetic,
                                     MetropolisAlgorithm::Metropolis,
                                     MetropolisAlgorithm::Random);

    viewControl->setAlgorithm(&mcCalculator);

    auto &backend = dynamic_cast<SFMLBackend&>(Core::BackendManager::GetBackend());
    backend.addSFMLListener(viewControl);
    backend.run(new DummyProgram());

    delete viewControl;

    return 0;
}