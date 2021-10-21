#include <FieldsApps/RtoR/Model/FunctionsCollection/AnalyticShockwave1D.h>
#include <FieldsApps/RtoR/View/Graphics/RtoROutputOpenGlShockwave.h>
#include "input-shockwave.h"

#include "FieldsApps/RtoR/Model/RtoRBoundaryCondition.h"
#include "FieldsApps/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "FieldsApps/RtoR/Model/FunctionsCollection/RegularDiracDelta.h"

#include "Core/Util/Workaround/StringStream.h"


using namespace RtoR;

InputShockwave::InputShockwave() : RtoRBCInterface("Shockwave in 1-dim") {
    addParameters({&a0, &E});
}

auto InputShockwave::getBoundary() const -> const void *
{
    //deltaType = vm["delta"].as<unsigned int>();

    auto a = 2*a0.value();

    const Real eps = a*a / (3*E.value());
    std::cout << "Shockwave eps = " << eps << std::endl;

    AnalyticShockwave1D shockwave1D(1);

    return new RtoR::BoundaryCondition(new RtoR::NullFunction,
                                             new RtoR::RegularDiracDelta(eps, a, RtoR::RegularDiracDelta::Regularization(deltaType)));
}

auto InputShockwave::_getDeltaTypeAsString() const -> String
{
    return (deltaType == 0 ? "gauss" : (deltaType == 1 ? "tri" : "unknown"));
}

OutputCollection InputShockwave::getOutputs(bool usingOpenGLBackend) const {
    RtoR::OutputOpenGLShockwave *oglout = nullptr;
    if(usingOpenGLBackend){
        return {new RtoR::OutputOpenGLShockwave(*a0 /2, *E)};
    }

    return {};
}

