//
// Created by joao on 04/04/2020.
//

#include "ManyOscillonsBuilder.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.h"
#include "Utils/RandUtils.h"

namespace Studios::Fields::RtoRThermal {

    using namespace Slab;
    using namespace Slab::Math;

    ManyOscillonsBuilder::ManyOscillonsBuilder()
            : Builder("Many oscillons", "General scattering of many oscillons.") {
        Interface->AddParameters({&nOscillons});
    }

    auto ManyOscillonsBuilder::getBoundary() -> Base::BoundaryConditions_ptr {
        int n = *nOscillons;
        auto L = kg_numeric_config->getL();
        auto xMin = kg_numeric_config->getxMin();

        RtoR::FunctionSummable initCondPhi, initCondDPhiDt;

        auto scale = L / n;
        for (int i = 0; i < n; i++) {
            auto tx = xMin + 1.05 * DevFloat(i) * scale;
            auto t = RandUtils::RandomUniformReal01();

            auto oscRight = RtoR::AnalyticOscillon(t, tx, scale, 0.9999, 0.7, 0.132487, false, false);
            auto oscLeft = RtoR::AnalyticOscillon(t, -tx, scale, .53, .7, 0.9324265, true, false);

            initCondPhi += oscRight + oscLeft;
            initCondDPhiDt += oscRight.swap() + oscLeft.swap();
        }

        auto proto = newFieldState();
        return New<KGRtoR::BoundaryCondition>(proto, initCondPhi.Clone(), initCondDPhiDt.Clone());
    }

/*
RtoR::Monitor *RtoR::ManyOscillonsBuilder::buildOpenGLOutput() {
    return new RtoR::Thermal::Monitor(simulationConfig.numericConfig, *(KGEnergy*)getHamiltonian());
}
 */


}