//
// Created by joao on 27/09/2021.
//

#include "InputSingleOscillon.h"

#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"
#include "Mappings/RtoR/Model/FunctionsCollection/AnalyticOscillon.h"

using namespace RtoR;

InputSingleOscillon::InputSingleOscillon() : Builder("Oscillon 1d", "Single 1+1 dim oscillon")
{
    interface->addParameters({v, V, alpha, lambda, mirror});
}

auto InputSingleOscillon::getBoundary() -> void * {
    AnalyticOscillon oscillon = AnalyticOscillon(0.0, *v, *V, *alpha, false, false);

    auto initCondPhi = oscillon;
    auto initCondDPhiDt = oscillon.swap();

    auto proto = (RtoR::EquationState*)newFieldState();

    return new BoundaryCondition(*proto, initCondPhi.Clone(), initCondDPhiDt.Clone());
}