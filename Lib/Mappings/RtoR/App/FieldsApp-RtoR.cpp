//
// Created by joao on 10/8/21.
//

#include "FieldsApp-RtoR.h"
#include "Mappings/RtoR/Model/RtoRFieldState.h"
#include "Mappings/RtoR/Core/RtoRModelAllocator.h"
#include "Mappings/RtoR/Core/RtoRModelAllocator_Langevin.h"
#include "Mappings/RtoR/Core/RtoRModelAllocator_Montecarlo.h"
#include "Mappings/SimulationBuilder.h"
#include "Base/Backend/GLUT/GLUTBackend.h"

#include "Base/Backend/Backend.h"

#include "Base/Controller/Interface/InterfaceSelector.h"

#include "Phys/Numerics/Program/Integrator.h"


SimulationsAppRtoR::SimulationsAppRtoR(int argc, const char **argv, Base::SimulationBuilder::Ptr simBuilder)
        : AppBase(argc, argv), simBuilder(simBuilder)
{
    simBuilder->registerAllocator();

    /*
    switch (integration) {
        case Integration::langevin:
            RtoRModelAllocator_Langevin::Choose();
            break;
        case Integration::montecarlo:
            RtoRModelAllocator_Montecarlo::Choose();
            break;
        default:
            RtoRModelAllocator::Choose();
            RtoRModelAllocator::SetPotential(RtoRModelAllocator::Potential::V);
    }
     */

    AppBase::parseCLArgs();
}

auto SimulationsAppRtoR::run() -> int {
    const auto *boundaryConditions = simBuilder->getBoundary();
    auto *output = simBuilder->buildOutputManager();

    auto numericMethod = integration==Integration::montecarlo ? NumericalIntegration::Montecarlo
                                                              : NumericalIntegration::RK4;

    auto program = NumericalIntegration::New<RtoR::FieldState>(boundaryConditions, output, numericMethod);

    Backend &backend = Backend::GetInstance();

    backend.run(program);

    Backend::Destroy();

    return 0;
}