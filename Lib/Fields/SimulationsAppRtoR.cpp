//
// Created by joao on 10/8/21.
//

#include "SimulationsAppRtoR.h"
#include "Studios/Math/Maps/RtoR/Core/RtoRModelAllocator.h"

#include <Studios/Backend/Backend.h>
#include <Studios/Controller/Console/CLInterfaceManager.h>
#include <Studios/Controller/Console/CLInterfaceSelector.h>

#include <Studios/Math/Integrator/Integrator.h>
#include <Fields/BCInterface.h>


SimulationsAppRtoR::SimulationsAppRtoR(int argc, const char **argv)
        : AppBase(argc, argv)
{
    RtoRModelAllocator::Choose();

    CommandLineInterfaceManager::Parse(argc, argv);
}

auto SimulationsAppRtoR::run() -> int {
    auto *bcInput = dynamic_cast<Base::BCInterface*>(CLInterfaceSelector::getInstance().getSelectedBCInterface());

    const auto *boundaryConditions = bcInput->getBoundary();
    auto *output = bcInput->buildOutputManager();

    auto *integrator = new NumericalIntegration(boundaryConditions, output);

    Backend::GetInstance()->run(integrator);
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
