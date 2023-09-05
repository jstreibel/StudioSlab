//
// Created by joao on 04/04/2020.
//

#include "ManyOscillonsBuilder.h"
#include "Maps/RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.h"
#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"
#include "Utils/RandUtils.h"
#include "../ThermalMonitor.h"


RtoR::ManyOscillonsBuilder::ManyOscillonsBuilder()
: RtoR::Thermal::Builder("Many oscillons", "General scattering of many oscillons.")
{
    interface->addParameters({&nOscillons});
}

auto RtoR::ManyOscillonsBuilder::getBoundary() -> void * {
    int n = *nOscillons;
    auto L = simulationConfig.numericConfig.getL();
    auto xMin = simulationConfig.numericConfig.getxMin();

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

RtoR::Monitor *RtoR::ManyOscillonsBuilder::buildOpenGLOutput() {
    return new RtoR::Thermal::Monitor(simulationConfig.numericConfig, *(KGEnergy*)getHamiltonian());
}
