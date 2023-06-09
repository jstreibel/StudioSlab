//
// Created by joao on 10/8/21.
//

#include "SimulationsAppR2ToR.h"
#include "Base/Controller/Interface/InterfaceSelector.h"
#include "Phys/Numerics/Program/Integrator.h"
#include "Mappings/R2toR/Model/FieldState.h"
#include "Base/Backend/Backend.h"

#include "Phys/Numerics/Output/StructureBuilder.h"

#include "Mappings/RtoR/View/OutputStructureBuilderRtoR.h"
#include "Mappings/R2toR/View/OutputStructureBuilderR2ToR.h"

#include "Mappings/R2toR/Core/Allocator_R2toR.h"



R2toR::App::Simulations::Simulations(int argc, const char **argv, Base::SimulationBuilder::Ptr simBuilder)
        : AppBase(argc, argv), builder(simBuilder)
{
    Allocator_R2toR::Choose();

    parseCLArgs();
}

auto R2toR::App::Simulations::run() -> int {
    auto boundaryConditions = builder->getBoundary();
    auto output             = builder->buildOutputManager();

    auto *program            = NumericalIntegration::New<R2toR::FieldState>(boundaryConditions, output);

    auto backend             = Backend::GetInstance();

    backend->run(program);
    Backend::Destroy();

    delete program;

    return 0;
}

