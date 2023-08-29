//
// Created by joao on 7/30/23.
//

#include "App.h"

#include "Core/Controller/Interface/InterfaceSelector.h"
#include "Math/Numerics/Program/Integrator.h"
#include "Models/KleinGordon/R2toR/EquationState.h"
#include "Core/Backend/Backend.h"

Simulation::App::App(int argc, const char **argv, Core::Simulation::VoidBuilder::Ptr simBuilder)
: AppBase(argc, argv), builder(simBuilder)
{
    parseCLArgs();
}

auto Simulation::App::run() -> int {
    auto program            = new NumericalIntegration(*builder.get());
    Backend &backend        = Backend::GetInstance();

    backend.run(program);

    Backend::Destroy();

    return 0;
}

