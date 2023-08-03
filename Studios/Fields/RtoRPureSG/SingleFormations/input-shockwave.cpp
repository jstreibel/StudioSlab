

#include "input-shockwave.h"

#include "Mappings/RtoR/Model/FunctionsCollection/AnalyticShockwave1D.h"

#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"
#include "Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Mappings/RtoR/Model/FunctionsCollection/RegularDiracDelta.h"

#include "Mappings/RtoR/View/Graphics/RtoROutputOpenGLShockwave.h"
#include "Models/KleinGordon/RtoR/KG-RtoRBuilder.h"


using namespace RtoR;

InputShockwave::InputShockwave() : Builder("1d shockwave", "Shockwave in 1-dim") {
    interface->addParameters({&a0, &E});
}

auto InputShockwave::getBoundary() -> void *
{
    //deltaType = vm["delta"].as<unsigned int>();

    auto a = 2* a0.getValue();

    const Real eps = a*a / (3* E.getValue());

    AnalyticShockwave1D shockwave1D(a0);
    auto proto = (RtoR::EquationState*)newFieldState();
    return new RtoR::BoundaryCondition(*proto, new RtoR::NullFunction,
                                             new RtoR::RegularDiracDelta(eps, a, RtoR::RegularDiracDelta::Regularization(deltaType)));
}

auto InputShockwave::_getDeltaTypeAsString() const -> Str
{
    return (deltaType == 0 ? "gauss" : (deltaType == 1 ? "tri" : "unknown"));
}

OutputCollection InputShockwave::getOutputs(bool usingOpenGLBackend) const {
    RtoR::ShockwaveMonitor *oglout = nullptr;
    if(usingOpenGLBackend){
        return {new RtoR::ShockwaveMonitor(numericParams, *a0 / 2, *E)};
    }

    return {};
}

