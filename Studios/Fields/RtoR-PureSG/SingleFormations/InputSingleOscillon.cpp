//
// Created by joao on 27/09/2021.
//

#include "InputSingleOscillon.h"

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoRBoundaryCondition.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.h"


namespace Studios::PureSG {

    using namespace Slab::Math;

    InputSingleOscillon::InputSingleOscillon() : Builder("Single 1d oscillon", "Single 1+1 dim oscillon") {
        interface->addParameters({&v, &V, &alpha, &lambda, &mirror});
    }

    auto InputSingleOscillon::getBoundary() -> Math::Base::BoundaryConditions_ptr {
        // RtoR::AnalyticOscillon oscillon =

        auto proto = newFieldState();

        return New<BoundaryCondition>(proto,
            RtoR::AnalyticOscillon(0.0, *v, *V, *alpha, false, false, RtoR::AnalyticOscillon::Bit::phi).Clone(),
            RtoR::AnalyticOscillon(0.0, *v, *V, *alpha, false, false, RtoR::AnalyticOscillon::Bit::dPhiDt).Clone());
    }


}