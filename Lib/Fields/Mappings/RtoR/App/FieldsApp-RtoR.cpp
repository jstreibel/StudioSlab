//
// Created by joao on 10/8/21.
//

#include "FieldsApp-RtoR.h"
#include "../Model/RtoRFieldState.h"
#include "../Core/RtoRModelAllocator.h"
#include "Fields/Mappings/BCInterface.h"

#include <Studios/Backend/Backend.h>

#include <Studios/Controller/Interface/InterfaceSelector.h>

#include <Phys/DiffMath/Numerics/Integrator.h>



SimulationsAppRtoR::SimulationsAppRtoR(int argc, const char **argv)
        : AppBase(argc, argv)
{
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

//auto SimulationsAppRtoR::getCommandLineOptions() -> CLOptionsDescription {
//    po::options_description desc("Options for simulation output structure.");
//
//    //TODO: colocar sub-menus aqui
//    desc.add_options()("gl,g", "Use OpenGL backend.");
//
//    auto xtra = OutputStructureBuilderRtoR::getCommandLineOptions();
//
//    return desc;
//}
