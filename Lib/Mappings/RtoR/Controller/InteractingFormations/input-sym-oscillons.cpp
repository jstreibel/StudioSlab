#include "input-sym-oscillons.h"

#include "Mappings/RtoR/Model/RtoRBoundaryCondition.h"
#include "Mappings/RtoR/Model/FunctionsCollection/AnalyticOscillon.h"

using namespace RtoR;

InputSymmetricOscillon::InputSymmetricOscillon()
: RtoRBCInterface("Symmetric scattering", "Symmetric / antisymmetric oscillon scattering in (1+1) dimensions.")
{
    interface->addParameters({&v, &V, &alpha, &isAntiSymmetric});
}

auto InputSymmetricOscillon::getBoundary() -> void *
{
    RtoR::FunctionSummable &initCondPhi = *(new RtoR::FunctionSummable),
                         &initCondDPhiDt = *(new RtoR::FunctionSummable);

    RtoR::AnalyticOscillon oscRight = RtoR::AnalyticOscillon(0.0, v.getValue(), V.getValue(), alpha.getValue(),
                                                             false, false);
    RtoR::AnalyticOscillon oscLeft  = RtoR::AnalyticOscillon(0.0, v.getValue(), V.getValue(), alpha.getValue(),
                                                             true, isAntiSymmetric.getValue());

    initCondPhi += oscRight+oscLeft;
    initCondDPhiDt += oscRight.swap() + oscLeft.swap();

    auto proto = (RtoR::EquationState*)newFieldState();
    return new RtoR::BoundaryCondition(*proto, &initCondPhi, &initCondDPhiDt);
}

