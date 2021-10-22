//
// Created by joao on 04/04/2020.
//

#include <Apps/Simulations/Maps/RtoR/Model/FunctionsCollection/AnalyticOscillon.h>
#include <Apps/Simulations/Maps/RtoR/Model/RtoRBoundaryCondition.h>
#include "InputManyOscillons.h"


RtoR::InputManyOscillons::InputManyOscillons() : RtoRBCInterface("General scattering of many oscillons.") {

}

auto RtoR::InputManyOscillons::getBoundary() const -> const void * {
    auto n = 100;

    auto L = 20.0;

    RtoR::FunctionSummable initCondPhi, initCondDPhiDt;

    auto scale = L/n;
    for(int i=0; i<n; i++){
        auto tx = -10. + 1.1*double(i)*scale;

        AnalyticOscillon oscRight = AnalyticOscillon(.0, tx, scale, 0.9999, 0.7, 0.132487, false, false);
        AnalyticOscillon oscLeft  = AnalyticOscillon(.0, -tx, scale, .53, .7, 0.9324265, true, false);

        initCondPhi += oscRight+oscLeft;
        initCondDPhiDt += oscRight.swap() + oscLeft.swap();
    }

    return new BoundaryCondition(initCondPhi.Clone(), initCondDPhiDt.Clone());
}
