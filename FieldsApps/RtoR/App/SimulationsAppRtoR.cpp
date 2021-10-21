//
// Created by joao on 10/8/21.
//

#include "SimulationsAppRtoR.h"


#include <Apps/Simulations/OutputStructureBuilders/OutputStructureBuilderBase.h>
#include <FieldsApps/RtoR/App/OutputStructureBuilderRtoR.h>
#include <FieldsApps/R2toR/App/OutputStructureBuilderR2ToR.h>

#include "Controller/Console/CommandLineInterfaceDefinitions.h"

#include <Core/NumericalIntegration.h>
#include <FieldsApps/RtoR/Core/RtoRModelAllocator.h>
#include <Controller/Console/CommandLineInterfaceManager.h>
#include "Core/Util/Workaround/StringStream.h"

#include "Apps/Backend/GLUTBackend.h"
#include "Apps/Backend/ConsoleBackend.h"
#include "Core/Device.h"


SimulationsAppRtoR::SimulationsAppRtoR(int argc, const char **argv)
        : AppBase(argc, argv)
{
    RtoRModelAllocator::Choose();

    CommandLineInterfaceManager::Parse(argc, argv);
}

auto SimulationsAppRtoR::run() -> int {
    auto *bcInput = CLInterfaceSelector::getInstance().getSelectedBCInterface();

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
