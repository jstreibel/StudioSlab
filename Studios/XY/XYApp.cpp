//
// Created by joao on 11/4/21.
//

#include "XYApp.h"

#include "Graphics/Backend/GraphicBackend.h"

#include "Graphics/Backend/SFML/SFMLBackend.h"
#include "Core/Backend/BackendManager.h"

#include "Math/Numerics/Metropolis/XYMetropolisAlgorithm.h"
#include "Models/XY/SingleSim/SingleSimViewController.h"
#include "../../Lib/Core/Controller/CommandLineArgsManager.h"
#include "Graphics/Backend/SFML/SFMLSystemWindow.h"
#include "Graphics/SlabGraphics.h"
#include "Core/SlabCore.h"

#define DONT_SELF_REGISTER false

XY::App::App(int argc, const char **argv) : Slab::Core::AppBase(argc, argv, DONT_SELF_REGISTER) {
    Interface->AddParameters({&N, &T, &MCSteps, &transient});

    RegisterToManager();

    Slab::Core::BackendManager::Startup("SFML");
    // Slab::Graphics::GetGraphicsBackend()->GetMainSystemWindow();
    Slab::Core::LoadModule("Nuklear");

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
    backend->Run();

    return 0;
}