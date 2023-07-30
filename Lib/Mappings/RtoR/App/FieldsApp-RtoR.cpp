//
// Created by joao on 10/8/21.
//

#include "FieldsApp-RtoR.h"
#include "Mappings/RtoR/Model/RtoRFieldState.h"
#include "Phys/Numerics/VoidBuilder.h"
#include "Base/Backend/GLUT/GLUTBackend.h"

#include "Phys/Numerics/Program/Integrator.h"



SimulationsAppRtoR::SimulationsAppRtoR(int argc, const char **argv, Base::Simulation::VoidBuilder::Ptr simBuilder)
        : AppBase(argc, argv), simBuilder(simBuilder)
{
    AppBase::parseCLArgs();
}

auto SimulationsAppRtoR::run() -> int {
    // auto program = new NumericalIntegration<RtoR::FieldState>(*simBuilder.get());
    auto program = new NumericalIntegration(*simBuilder.get());

    Backend &backend = Backend::GetInstance();

    backend.run(program);

    Backend::Destroy();

    return 0;
}