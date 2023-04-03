//
// Created by joao on 10/8/21.
//

#include "FieldsApp-RtoR.h"
#include "../Model/RtoRFieldState.h"
#include "../Core/RtoRModelAllocator.h"
#include "../Core/RtoRModelAllocator_Langevin.h"
#include "../Core/RtoRModelAllocator_Montecarlo.h"
#include "Fields/Mappings/BCInterface.h"
#include "Studios/Backend/GLUT/GLUTBackend.h"

#include <Studios/Backend/Backend.h>

#include <Studios/Controller/Interface/InterfaceSelector.h>

#include <Phys/Numerics/Program/Integrator.h>



SimulationsAppRtoR::SimulationsAppRtoR(int argc, const char **argv, Integration integration)
        : AppBase(argc, argv), integration(integration)
{
    switch (integration) {
        case Integration::langevin:
            RtoRModelAllocator_Langevin::Choose();
            break;
        case Integration::montecarlo:
            RtoRModelAllocator_Montecarlo::Choose();
            break;
        default:
            RtoRModelAllocator::Choose();

            RtoRModelAllocator::SetPotential(RtoRModelAllocator::Potential::free);
    }

    AppBase::parseCLArgs();
}

auto SimulationsAppRtoR::run() -> int {
    auto *bcInput = dynamic_cast<Base::BCInterface*>(InterfaceSelector::getInstance().getCurrentCandidate());

    const auto *boundaryConditions = bcInput->getBoundary();
    auto *output = bcInput->buildOutputManager();


    auto numericMethod = integration==Integration::montecarlo ? NumericalIntegration::Montecarlo
                                                              : NumericalIntegration::RK4;
    auto *integrator = NumericalIntegration::New<RtoR::FieldState>(boundaryConditions, output, numericMethod);

    auto backend = Backend::GetInstance();

    if (backend->backendName == "GLUT backend") {
        GLUTBackend::GetInstance()->setStepsPerFrame(80);
    }

    backend->run(integrator);
    Backend::Destroy();

    delete integrator;

    return 0;
}