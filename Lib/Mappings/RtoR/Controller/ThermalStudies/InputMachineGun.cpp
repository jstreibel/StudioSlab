//
// Created by joao on 04/04/2020.
//

#include "InputMachineGun.h"

#include "Mappings/RtoR/Model/FunctionsCollection/AnalyticOscillon.h"
#include "Mappings/RtoR/Model/RtoRBoundaryCondition.h"


RtoR::InputMachineGun::InputMachineGun() : RtoRBCInterface("Machinegun scattering") {
    addParameters({&v1, &n});

}

auto RtoR::InputMachineGun::getBoundary() const -> const void * {
    RtoR::FunctionSummable initCondPhi, initCondDPhiDt;

    auto scaleBig = 1.0;
    auto scaleSmall = 0.25;

    auto BigOne = AnalyticOscillon(.0, -scaleBig, scaleBig, 0, *v1, 0, false, false);
    initCondPhi += BigOne;
    initCondDPhiDt += BigOne.swap();

    for(int i=0; i<*n; i++){
        auto tx = double(i)*.2;

        auto tiny = AnalyticOscillon(.0, tx, scaleSmall, 0, .75, 0.2634, false, false);

        initCondPhi += tiny;
        initCondDPhiDt += tiny.swap();
    }

    return new BoundaryCondition(initCondPhi.Clone(), initCondDPhiDt.Clone());
}
