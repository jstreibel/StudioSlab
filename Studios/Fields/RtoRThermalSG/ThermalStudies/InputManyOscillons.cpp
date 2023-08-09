//
// Created by joao on 04/04/2020.
//

#include "InputManyOscillons.h"
#include "Mappings/RtoR/Model/FunctionsCollection/AnalyticOscillon.h"
#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"
#include "Common/RandUtils.h"
#include "../ThermalMonitor.h"


RtoR::InputManyOscillons::InputManyOscillons()
: Builder("Many oscillons", "General scattering of many oscillons.")
{
    interface->addParameters({&nOscillons});
}

auto RtoR::InputManyOscillons::getBoundary() -> void * {
    int n = nOscillons;
    auto L = numericParams.getL();
    auto xMin = numericParams.getxMin();

    RtoR::FunctionSummable initCondPhi, initCondDPhiDt;

    auto scale = L/n;
    for(int i=0; i<n; i++){
        auto tx = xMin + 1.05*Real(i)*scale;
        auto t = RandUtils::random01();

        AnalyticOscillon oscRight = AnalyticOscillon(t, tx, scale, 0.9999, 0.7, 0.132487, false, false);
        AnalyticOscillon oscLeft  = AnalyticOscillon(t, -tx, scale, .53, .7, 0.9324265, true, false);

        initCondPhi += oscRight+oscLeft;
        initCondDPhiDt += oscRight.swap() + oscLeft.swap();
    }

    auto proto = (RtoR::EquationState*)newFieldState();
    return new BoundaryCondition(*proto, initCondPhi.Clone(), initCondDPhiDt.Clone());
}

RtoR::Monitor *RtoR::InputManyOscillons::buildOpenGLOutput() {
    return new RtoR::Thermal::Monitor(numericParams, *(KGEnergy*)getHamiltonian());
}
