//
// Created by joao on 7/19/19.
//

#include "input-general-oscillons.h"
#include "FieldsApps/RtoR/Model/RtoRBoundaryCondition.h"
#include "FieldsApps/RtoR/Model/FunctionsCollection/AnalyticOscillon.h"

#include "Core/Util/Workaround/StringStream.h"

using namespace RtoR;


InputGeneralOscillons::InputGeneralOscillons()
: RtoRBCInterface("General scattering of any two oscillons.")
{
    addParameters({&v1, &v2, &alpha1, &alpha2, &l, &V, &xTouchLocation, &mirror});
}

auto InputGeneralOscillons::getBoundary() const -> const void *
{
    AnalyticOscillon oscRight = AnalyticOscillon(0.0, v1.value(), V.value(), alpha1.value(),
                                                 false, false);
    AnalyticOscillon oscLeft  = AnalyticOscillon(0.0, v2.value(), V.value(), alpha2.value(),
                                                 true, mirror.value());

    auto initCondPhi = oscRight + oscLeft;
    auto initCondDPhiDt = oscRight.swap() + oscLeft.swap();

    return new BoundaryCondition(initCondPhi.Clone(), initCondDPhiDt.Clone());
}

