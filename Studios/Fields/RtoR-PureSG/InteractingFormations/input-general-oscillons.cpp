//
// Created by joao on 7/19/19.
//

#include "input-general-oscillons.h"

#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"
#include "Mappings/RtoR/Model/FunctionsCollection/AnalyticOscillon.h"
#include "Models/KleinGordon/RtoR/KG-RtoRBuilder.h"

using namespace RtoR;


InputGeneralOscillons::InputGeneralOscillons()
: KGBuilder("Scattering", "General scattering of any two oscillons.")
{
    interface->addParameters({&v1, &v2, &alpha1, &alpha2, &l, &V, &xTouchLocation, &mirror});
}

auto InputGeneralOscillons::getBoundary() -> void *
{
    AnalyticOscillon oscRight = AnalyticOscillon(0.0, v1.getValue(), V.getValue(), alpha1.getValue(),
                                                 false, false);
    AnalyticOscillon oscLeft  = AnalyticOscillon(0.0, v2.getValue(), V.getValue(), alpha2.getValue(),
                                                 true, mirror.getValue());

    auto initCondPhi = oscRight + oscLeft;
    auto initCondDPhiDt = oscRight.swap() + oscLeft.swap();

    auto proto = (RtoR::EquationState*)newFieldState();
    return new BoundaryCondition(*proto, initCondPhi.Clone(), initCondDPhiDt.Clone());
}

