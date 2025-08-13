//
// Created by joao on 04/04/2020.
//

#include "MachineGunBuilder.h"

#include "Math/Function/RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.h"
#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoRBoundaryCondition.h"

namespace Studios::Fields::RtoRThermal {

    using namespace Slab;
    using namespace Slab::Math;

    MachineGunBuilder::MachineGunBuilder() : Builder("Machinegun", "Machinegun scattering") {
        Interface->AddParameters({&v1, &n});

    }

    auto MachineGunBuilder::GetBoundary() -> Base::BoundaryConditions_ptr {
        RtoR::FunctionSummable initCondPhi, initCondDPhiDt;

        auto scaleBig = 1.0;
        auto scaleSmall = 0.25;

        auto BigOne = RtoR::AnalyticOscillon(.0, -scaleBig, scaleBig, 0, *v1, 0, false, false);
        initCondPhi += BigOne;
        initCondDPhiDt += BigOne.swap();

        for (int i = 0; i < *n; i++) {
            auto tx = DevFloat(i) * .2;

            auto tiny = RtoR::AnalyticOscillon(.0, tx, scaleSmall, 0, .75, 0.2634, false, false);

            initCondPhi += tiny;
            initCondDPhiDt += tiny.swap();
        }

        auto proto = NewFieldState();
        return New<Slab::Models::KGRtoR::BoundaryCondition>(proto, initCondPhi.Clone(), initCondDPhiDt.Clone());
    }


}