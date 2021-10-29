//
// Created by joao on 10/8/21.
//

#include "SimulationsAppR2ToR.h"


#include <Apps/Simulations/OutputStructureBuilders/OutputStructureBuilderBase.h>
#include <Lib/Fields/Maps/RtoR/View/OutputStructureBuilderRtoR.h>
#include <Studios/Math/Maps/R2toR/View/OutputStructureBuilderR2ToR.h>

#include "Controller/Console/CommandLineInterfaceDefinitions.h"

#include <Studios/Math/Integrator/Integrator.h>
#include "Core/Util/Workaround/StringStream.h"

#include "Apps/Backend/GLUTBackend.h"
#include "Apps/Backend/ConsoleBackend.h"
#include "Studios/Math/Device.h"


SimulationsAppR2toR::SimulationsAppR2toR(int argc, const char **argv)
        : SimulationsApp(argc, argv)
{
    CLOptionsDescription options = _buildFullCommandLineOptions(); // essa funcao depende de o boundaryConditionsInput estar devidamente inicializado.

    namespace po = boost::program_options;
    po::store(po::parse_command_line(argc, argv, options), vm);

}

auto SimulationsAppR2toR::run() -> int {
    boundaryConditionsInput->YoureTheChosenOne(vm);
    std::cout << "Sim: " << boundaryConditionsInput->getGeneralDescription() << std::endl;


    NumericParams p(vm);
    Device dev(vm);
    Allocator::Initialize(p, dev, vm, boundaryConditionsInput->getModelParams());


    bool useOpenGLBackend = vm.count("gl") != 0;
    if (useOpenGLBackend) backend = GLUTBackend::GetInstance();
    else backend = ConsoleBackend::getSingleton();


    OutputManager output;
    OutputStructureBuilderR2toR outputBuilder;
    outputBuilder.build(output, boundaryConditionsInput->getInterfaceParametersMap(), vm, useOpenGLBackend);


    auto *_integrator = new NumericalIntegration(boundaryConditionsInput->getBoundary(), &output);
    this->integrator = _integrator;


    // BACKEND NAO EH DELETADO! Ele eh um singleton.
    backend->run(integrator);

    output.notifyIntegrationFinished(_integrator->getOutputInfo());

    delete integrator;
    GLUTBackend::Destroy();
    delete boundaryConditionsInput;

    return 0;
}

auto SimulationsAppR2toR::getCommandLineOptions() -> CLOptionsDescription {
    return CLOptionsDescription();
}
