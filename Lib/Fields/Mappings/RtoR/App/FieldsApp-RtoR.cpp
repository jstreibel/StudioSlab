//
// Created by joao on 10/8/21.
//

#include "FieldsApp-RtoR.h"
#include "../Model/RtoRFieldState.h"
#include "../Core/RtoRModelAllocator.h"
#include "../Core/RtoRModelAllocator_Langevin.h"
#include "Fields/Mappings/BCInterface.h"

#include <Studios/Backend/Backend.h>

#include <Studios/Controller/Interface/InterfaceSelector.h>

#include <Phys/Numerics/Program/Integrator.h>



SimulationsAppRtoR::SimulationsAppRtoR(int argc, const char **argv, bool thermalHamiltonian)
        : AppBase(argc, argv)
{
    if(thermalHamiltonian)
        RtoRModelAllocator_Langevin::Choose();
    else
        RtoRModelAllocator::Choose();

    AppBase::parseCLArgs();
}

auto SimulationsAppRtoR::run() -> int {
    auto *bcInput = dynamic_cast<Base::BCInterface*>(InterfaceSelector::getInstance().getCurrentCandidate());

    const auto *boundaryConditions = bcInput->getBoundary();
    auto *output = bcInput->buildOutputManager();

    auto *integrator = NumericalIntegration::New<RtoR::FieldState>(boundaryConditions, output);

    auto backend = Backend::GetInstance();
    backend->run(integrator);
    Backend::Destroy();

    delete integrator;

    return 0;
}