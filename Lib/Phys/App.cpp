//
// Created by joao on 7/30/23.
//

#include "App.h"

#include "Base/Controller/Interface/InterfaceSelector.h"
#include "Phys/Numerics/Program/Integrator.h"
#include "Mappings/R2toR/Model/EquationState.h"
#include "Base/Backend/Backend.h"

Simulation::App::App(int argc, const char **argv, Base::Simulation::VoidBuilder::Ptr simBuilder)
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

