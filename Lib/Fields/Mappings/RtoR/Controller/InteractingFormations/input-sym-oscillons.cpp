#include "input-sym-oscillons.h"

#include "../../Model/RtoRBoundaryCondition.h"
#include "../../Model/FunctionsCollection/AnalyticOscillon.h"

#include <Common/Workaround/StringStream.h>

using namespace RtoR;

InputSymmetricOscillon::InputSymmetricOscillon()
: RtoRBCInterface("Symmetric / antisymmetric oscillon scattering in (1+1) dimensions.")
{
    addParameters({&v, &V, &alpha, &isAntiSymmetric});
}

auto InputSymmetricOscillon::getBoundary() const -> const void *
{
    RtoR::FunctionSummable &initCondPhi = *(new RtoR::FunctionSummable),
                         &initCondDPhiDt = *(new RtoR::FunctionSummable);

    RtoR::AnalyticOscillon oscRight = RtoR::AnalyticOscillon(0.0, v.value(), V.value(), alpha.value(),
                                                             false, false);
    RtoR::AnalyticOscillon oscLeft  = RtoR::AnalyticOscillon(0.0, v.value(), V.value(), alpha.value(),
                                                             true, isAntiSymmetric.value());

    initCondPhi += oscRight+oscLeft;
    initCondDPhiDt += oscRight.swap() + oscLeft.swap();

    return new RtoR::BoundaryCondition(&initCondPhi, &initCondDPhiDt);
}
