

#include "input-shockwave.h"

#include "Maps/RtoR/Model/FunctionsCollection/Shockwave/AnalyticShockwave1D.h"

#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"
#include "Maps/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Maps/RtoR/Model/FunctionsCollection/RegularDiracDelta.h"



using namespace RtoR;

InputShockwave::InputShockwave() : KGBuilder("1d shockwave", "Shockwave in 1-dim") {
    interface->addParameters({&a0, &E});
}

auto InputShockwave::getBoundary() -> void *
{
    //deltaType = vm["delta"].as<unsigned int>();

    auto a = 2* a0.getValue();

    const Real eps = a*a / (3* E.getValue());

    AnalyticShockwave1D shockwave1D(*a0);
    auto proto = (RtoR::EquationState*)newFieldState();
    return new RtoR::BoundaryCondition(*proto, new RtoR::NullFunction,
                                             new RtoR::RegularDiracDelta(eps, a, RtoR::RegularDiracDelta::Regularization(deltaType)));
}

