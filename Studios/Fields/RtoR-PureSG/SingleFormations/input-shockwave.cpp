

#include "input-shockwave.h"

#include "Mappings/RtoR/Model/FunctionsCollection/AnalyticShockwave1D.h"

#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"
#include "Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Mappings/RtoR/Model/FunctionsCollection/RegularDiracDelta.h"

#include "Mappings/RtoR/View/Graphics/RtoROutputOpenGLShockwave.h"
#include "Models/KleinGordon/RtoR/KG-RtoRBuilder.h"


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

