//
// Created by joao on 10/8/21.
//

#include "R2toR App.h"
#include "Base/Controller/Interface/InterfaceSelector.h"
#include "Phys/Numerics/Program/Integrator.h"
#include "Mappings/R2toR/Model/EquationState.h"
#include "Base/Backend/Backend.h"

#include "Mappings/R2toR/Core/R2toR_Allocator.h"


R2toR::App::Simulations::Simulations(int argc, const char **argv, Base::SimulationBuilder::Ptr simBuilder)
        : AppBase(argc, argv), builder(simBuilder)
{
    // Allocator precisa ser registrado aqui, para montar esquema de opções de linha de comando pro usuário
    builder->registerAllocator();

    parseCLArgs();
}

auto R2toR::App::Simulations::run() -> int {

    auto boundaryConditions = builder->getBoundary();
    auto output             = builder->buildOutputManager();

    auto program            = NumericalIntegration::New<R2toR::EquationState>(boundaryConditions, output);
    Backend &backend        = Backend::GetInstance();

    backend.run(program);

    Backend::Destroy();
    delete program;

    return 0;
}

