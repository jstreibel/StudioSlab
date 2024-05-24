//
// Created by joao on 27/09/2021.
//

#include "InputSingleOscillon.h"

#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.h"


namespace Studios::PureSG {

    using namespace Slab::Math;

    InputSingleOscillon::InputSingleOscillon() : Builder("Oscillon 1d", "Single 1+1 dim oscillon") {
        interface->addParameters({&v, &V, &alpha, &lambda, &mirror});
    }

    auto InputSingleOscillon::getBoundary() -> void * {
        RtoR::AnalyticOscillon oscillon = RtoR::AnalyticOscillon(0.0, *v, *V, *alpha, false, false, RtoR::AnalyticOscillon::Bit::phi);

        auto initCondPhi = oscillon;
        auto initCondDPhiDt = oscillon.swap();

        auto proto = (EquationState *) newFieldState();

        return new BoundaryCondition(*proto, initCondPhi.Clone(), initCondDPhiDt.Clone());
    }


}