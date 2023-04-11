//
// Created by joao on 4/11/23.
//


#include "LeadingDelta.h"
#include "Fields/Mappings/R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.h"

namespace R2toR {
    namespace LeadingDelta {
        SpecialRingDelta *ringDelta1 = nullptr;
    }
}

void R2toR::LeadingDelta::OutGL::draw() {
    if (!lastData.hasValidData()) return;

    std::stringstream ss;
    auto &p = Numerics::Allocator::getInstance().getNumericParams();
    const auto t = lastData.getSimTime();
    const auto L = p.getL();
    const auto N = p.getN();

    auto dt = Numerics::Allocator::getInstance().getNumericParams().getdt();
    stats.addVolatileStat(String("t = ") + ToString(t));
    stats.addVolatileStat(String("L = ") + ToString(L));
    stats.addVolatileStat(String("N = ") + ToString(N));
    stats.addVolatileStat(String("N² = ") + ToString(N*N));

    const R2toR::FieldState &fState = *lastData.getFieldData<R2toR::FieldState>();
    auto &phi = fState.getPhi();

    mSectionGraph.clearFunctions();
    mSectionGraph.addFunction(&phi, ColorScheme::graphs[0], "phi");

    auto rd = R2toR::LeadingDelta::ringDelta1;
    stats.addVolatileStat(String("Ring radius: ") + ToString(rd->getRadius()));
    mSectionGraph.addFunction(rd, ColorScheme::graphs[2], "Ring delta");

    RtoR::AnalyticShockwave2DRadialSymmetry radialShockwave;
    radialShockwave.sett(rd->getRadius() - dt);
    FunctionAzimuthalSymmetry shockwave(&radialShockwave, 1, 0, 0, false);
    mSectionGraph.addFunction(&shockwave, ColorScheme::graphs[3], "Analytic");

    panel->draw(true, true);
}

IntPair R2toR::LeadingDelta::OutGL::getWindowSizeHint() {
    return {3260, 1280};
}

