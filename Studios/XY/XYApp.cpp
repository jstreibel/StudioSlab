//
// Created by joao on 11/4/21.
//

#include "XYApp.h"

#include "Graphics/Backend/GraphicBackend.h"

#include "Graphics/Backend/SFML/SFMLBackend.h"
#include "Core/Backend/BackendManager.h"

#include "Math/Numerics/Metropolis/XYMetropolisAlgorithm.h"
#include "Models/XY/SingleSim/SingleSimViewController.h"
#include "Core/Controller/CommandLine/CLArgsManager.h"
#include "Graphics/Backend/SFML/SFMLSystemWindow.h"
#include "Graphics/SlabGraphics.h"

#define DONT_SELF_REGISTER false

XY::App::App(int argc, const char **argv) : Slab::Core::AppBase(argc, argv, DONT_SELF_REGISTER) {
    interface->addParameters({&N, &T, &MCSteps, &transient});

    registerToManager();

    Slab::Core::BackendManager::Startup   ("SFML");
    Slab::Core::BackendManager::LoadModule("Nuklear");

    Slab::Core::CLArgsManager::Parse(argc, argv);
}


int XY::App::App::run() {

    auto viewControl = Slab::New<Slab::Lost::ThermoOutput::SingleSimViewController>(*N, *MCSteps, *transient);

    using Algorithm = Slab::Math::XYMetropolisAlgorithm;
    Slab::Math::XYMetropolisAlgorithm mcCalculator(*N, *T, .0,
                                                   Algorithm::InitialConditions::Ferromagnetic,
                                                   Algorithm::Dynamic::Metropolis,
                                                   Algorithm::Sweeping::Random);

    viewControl->setAlgorithm(&mcCalculator);

    auto backend = Slab::Graphics::GetGraphicsBackend();
    Slab::DynamicPointerCast<Slab::Graphics::SFMLSystemWindow>
            (backend->GetMainSystemWindow())->addSFMLListener(viewControl);
    backend->run();

    return 0;
}