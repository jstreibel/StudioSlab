//
// Created by joao on 04/04/2020.
//

#include "MachineGunBuilder.h"

#include "Maps/RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.h"
#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"

RtoR::MachineGunBuilder::MachineGunBuilder() : RtoR::Thermal::Builder("Machinegun", "Machinegun scattering") {
    interface->addParameters({&v1, &n});

}

auto RtoR::MachineGunBuilder::getBoundary() -> void * {
    RtoR::FunctionSummable initCondPhi, initCondDPhiDt;

    auto scaleBig = 1.0;
    auto scaleSmall = 0.25;

    auto BigOne = AnalyticOscillon(.0, -scaleBig, scaleBig, 0, *v1, 0, false, false);
    initCondPhi += BigOne;
    initCondDPhiDt += BigOne.swap();

    for(int i=0; i<*n; i++){
        auto tx = Real(i)*.2;

        auto tiny = AnalyticOscillon(.0, tx, scaleSmall, 0, .75, 0.2634, false, false);

        initCondPhi += tiny;
        initCondDPhiDt += tiny.swap();
    }

    auto proto = (RtoR::EquationState*)newFieldState();
    return new BoundaryCondition(*proto, initCondPhi.Clone(), initCondDPhiDt.Clone());
}
