//
// Created by joao on 7/19/19.
//

#include "input-general-oscillons.h"

#include "Mappings/RtoR/Model/RtoRBoundaryCondition.h"
#include "Mappings/RtoR/Model/FunctionsCollection/AnalyticOscillon.h"

using namespace RtoR;


InputGeneralOscillons::InputGeneralOscillons()
: RtoRBCInterface("General scattering of any two oscillons.")
{
    interface->addParameters({&v1, &v2, &alpha1, &alpha2, &l, &V, &xTouchLocation, &mirror});
}

auto InputGeneralOscillons::getBoundary() const -> const void *
{
    AnalyticOscillon oscRight = AnalyticOscillon(0.0, v1.getValue(), V.getValue(), alpha1.getValue(),
                                                 false, false);
    AnalyticOscillon oscLeft  = AnalyticOscillon(0.0, v2.getValue(), V.getValue(), alpha2.getValue(),
                                                 true, mirror.getValue());

    auto initCondPhi = oscRight + oscLeft;
    auto initCondDPhiDt = oscRight.swap() + oscLeft.swap();

    return new BoundaryCondition(initCondPhi.Clone(), initCondDPhiDt.Clone());
}

