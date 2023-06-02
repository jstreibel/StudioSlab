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



SimulationsAppR2toR::SimulationsAppR2toR(int argc, const char **argv)
        : AppBase(argc, argv)
{
    Allocator_R2toR::Choose();

    //AppBase::parseCLArgs();
    parseCLArgs();
}

auto SimulationsAppR2toR::run() -> int {
    auto *bcInput = dynamic_cast<Base::BCBuilder*>(InterfaceSelector::getInstance().getCurrentCandidate());

    const auto *boundaryConditions = bcInput->getBoundary();
    auto *output = bcInput->buildOutputManager();

    auto *integrator = NumericalIntegration::New<R2toR::FieldState>(boundaryConditions, output);

    auto backend = Backend::GetInstance();
    backend->run(integrator);
    Backend::Destroy();

    delete integrator;

    return 0;
}

