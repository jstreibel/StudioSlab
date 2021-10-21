//
// Created by joao on 27/09/2021.
//

#include "InputSingleOscillon.h"

#include "FieldsApps/RtoR/Model/RtoRBoundaryCondition.h"
#include "FieldsApps/RtoR/Model/FunctionsCollection/AnalyticOscillon.h"

#include "Core/Util/Workaround/StringStream.h"

using namespace RtoR;

InputSingleOscillon::InputSingleOscillon() : RtoRBCInterface("Single oscillon")
{
    addParameters({&v, &V, &alpha, &lambda, &mirror});
}

auto InputSingleOscillon::getBoundary() const -> const void * {
    AnalyticOscillon oscillon = AnalyticOscillon(0.0, *v, *V, *alpha, false, false);

    auto initCondPhi = oscillon;
    auto initCondDPhiDt = oscillon.swap();

    return new BoundaryCondition(initCondPhi.Clone(), initCondDPhiDt.Clone());
}