//
// Created by joao on 27/09/2021.
//

#include "InputSingleOscillon.h"

#include "Mappings/RtoR/Model/RtoRBoundaryCondition.h"
#include "Mappings/RtoR/Model/FunctionsCollection/AnalyticOscillon.h"

using namespace RtoR;

InputSingleOscillon::InputSingleOscillon() : RtoRBCInterface("Oscillon 1d", "Single 1+1 dim oscillon")
{
    interface->addParameters({v, V, alpha, lambda, mirror});
}

auto InputSingleOscillon::getBoundary() const -> const void * {
    AnalyticOscillon oscillon = AnalyticOscillon(0.0, *v, *V, *alpha, false, false);

    auto initCondPhi = oscillon;
    auto initCondDPhiDt = oscillon.swap();

    return new BoundaryCondition(initCondPhi.Clone(), initCondDPhiDt.Clone());
}