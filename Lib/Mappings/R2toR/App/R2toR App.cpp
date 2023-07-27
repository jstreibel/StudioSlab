//
// Created by joao on 10/8/21.
//

#include "R2toR App.h"
#include "Base/Controller/Interface/InterfaceSelector.h"
#include "Phys/Numerics/Program/Integrator.h"
#include "Mappings/R2toR/Model/EquationState.h"
#include "Base/Backend/Backend.h"


R2toR::App::Simulations::Simulations(int argc, const char **argv, Base::Simulation::VoidBuilder::Ptr simBuilder)
        : AppBase(argc, argv), builder(simBuilder)
{
    parseCLArgs();
}

auto R2toR::App::Simulations::run() -> int {

    R2toR::EquationState *fstate = nullptr;
    auto program            = new NumericalIntegration(*builder.get(), fstate);
    Backend &backend        = Backend::GetInstance();

    backend.run(program);

    Backend::Destroy();

    return 0;
}

