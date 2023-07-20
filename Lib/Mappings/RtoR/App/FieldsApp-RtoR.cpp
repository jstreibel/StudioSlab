//
// Created by joao on 10/8/21.
//

#include "FieldsApp-RtoR.h"
#include "Mappings/RtoR/Model/RtoRFieldState.h"
#include "Phys/Numerics/Builder.h"
#include "Base/Backend/GLUT/GLUTBackend.h"

#include "Phys/Numerics/Program/Integrator.h"



SimulationsAppRtoR::SimulationsAppRtoR(int argc, const char **argv, Base::Simulation::Builder::Ptr simBuilder)
        : AppBase(argc, argv), simBuilder(simBuilder)
{
    AppBase::parseCLArgs();
}

auto SimulationsAppRtoR::run() -> int {
    const auto *boundaryConditions = simBuilder->getBoundary();
    auto *output = simBuilder->buildOutputManager();

    // auto program = new NumericalIntegration<RtoR::FieldState>(*simBuilder.get());
    RtoR::FieldState *fstate = nullptr;
    auto program = new NumericalIntegration(*simBuilder.get(), fstate);

    Backend &backend = Backend::GetInstance();

    backend.run(program);

    Backend::Destroy();

    return 0;
}