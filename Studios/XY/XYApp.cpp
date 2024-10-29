//
// Created by joao on 11/4/21.
//

#include "XYApp.h"

#include "Graphics/Backend/GraphicBackend.h"

#include "Graphics/Backend/SFML/SFMLBackend.h"
#include "Core/Backend/BackendManager.h"

#include "Math/Numerics/Metropolis/XYMetropolisAlgorithm.h"
#include "Models/XY/SingleSim/SingleSimViewController.h"

#define DONT_SELF_REGISTER false

XY::App::App(int argc, const char **argv) : Slab::Core::AppBase(argc, argv, DONT_SELF_REGISTER) {
    interface->addParameters({&N, &T, &MCSteps, &transient});

    registerToManager();

    Slab::Core::BackendManager::Startup   ("SFML");
    Slab::Core::BackendManager::LoadModule("Nuklear");

    AppBase::parseCLArgs();
}


int XY::App::App::run() {

    auto viewControl = Slab::New<Slab::Lost::ThermoOutput::SingleSimViewController>(*N, *MCSteps, *transient);

    using Algorithm = Slab::Math::XYMetropolisAlgorithm;
    Slab::Math::XYMetropolisAlgorithm mcCalculator(*N, *T, .0,
                                                   Algorithm::InitialConditions::Ferromagnetic,
                                                   Algorithm::Dynamic::Metropolis,
                                                   Algorithm::Sweeping::Random);

    viewControl->setAlgorithm(&mcCalculator);

    auto &backend = dynamic_cast<Slab::Graphics::SFMLBackend&>(Slab::Core::BackendManager::GetBackend());
    backend.addSFMLListener(viewControl);
    backend.run();

    return 0;
}