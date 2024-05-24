//
// Created by joao on 04/04/2020.
//

#include "ManyOscillonsBuilder.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.h"
#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"
#include "Utils/RandUtils.h"

namespace Studios::Fields::RtoRThermal {

    using namespace Slab;
    using namespace Slab::Math;

    ManyOscillonsBuilder::ManyOscillonsBuilder()
            : Builder("Many oscillons", "General scattering of many oscillons.") {
        interface->addParameters({&nOscillons});
    }

    auto ManyOscillonsBuilder::getBoundary() -> void * {
        int n = *nOscillons;
        auto L = simulationConfig.numericConfig.getL();
        auto xMin = simulationConfig.numericConfig.getxMin();

        RtoR::FunctionSummable initCondPhi, initCondDPhiDt;

        auto scale = L / n;
        for (int i = 0; i < n; i++) {
            auto tx = xMin + 1.05 * Real(i) * scale;
            auto t = RandUtils::random01();

            auto oscRight = RtoR::AnalyticOscillon(t, tx, scale, 0.9999, 0.7, 0.132487, false, false);
            auto oscLeft = RtoR::AnalyticOscillon(t, -tx, scale, .53, .7, 0.9324265, true, false);

            initCondPhi += oscRight + oscLeft;
            initCondDPhiDt += oscRight.swap() + oscLeft.swap();
        }

        auto proto = (Models::KGRtoR::EquationState *) newFieldState();
        return new Models::KGRtoR::BoundaryCondition(*proto, initCondPhi.Clone(), initCondDPhiDt.Clone());
    }

/*
RtoR::Monitor *RtoR::ManyOscillonsBuilder::buildOpenGLOutput() {
    return new RtoR::Thermal::Monitor(simulationConfig.numericConfig, *(KGEnergy*)getHamiltonian());
}
 */


}