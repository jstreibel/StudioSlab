//
// Created by joao on 23/09/2019.
//

#ifndef GROWING_HOLE_GL_OUTPUT
#define GROWING_HOLE_GL_OUTPUT

#include "Fields/Mappings/R2toR/View/R2toROutputOpenGLGeneric.h"

#include "Phys/Numerics/Allocator.h"
#include "Fields/Mappings/R2toR/Model/FieldState.h"
#include "Fields/Mappings/R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.h"
#include "Fields/Mappings/R2toR/Model/FunctionsCollection/FunctionAzimuthalSymmetry.h"

namespace R2toR {
    class GrowingHoleOutGL : public R2toR::OutputOpenGL {
    public:
        GrowingHoleOutGL(Real xMin, Real xMax, Real yMin, Real yMax, Real phiMin, Real phiMax)
                : R2toR::OutputOpenGL(xMin, xMax, yMin, yMax, phiMin, phiMax) {   }

        void draw() override {
            if(!lastData.hasValidData()) return;

            std::stringstream ss;
            const Real t = lastData.getSimTime();
            const Real L = Numerics::Allocator::getInstance().getNumericParams().getL();
            const Real xMin = Numerics::Allocator::getInstance().getNumericParams().getxLeft();

            auto dt = Numerics::Allocator::getInstance().getNumericParams().getdt();
            stats.addVolatileStat(std::string("t = ") + std::to_string(getLastSimTime()));
            stats.addVolatileStat(std::string("L = ") + std::to_string(L));
            stats.addVolatileStat(std::string("xMin = ") + std::to_string(xMin));

            const R2toR::FieldState& fState = *lastData.getFieldData<R2toR::FieldState>();
            auto &phi = fState.getPhi();

            mSectionGraph.clearFunctions();
            mSectionGraph.addFunction(&phi, "Numeric", Styles::GetColorScheme()->funcPlotStyles[0]);
            RtoR::AnalyticShockwave2DRadialSymmetry radialShockwave; radialShockwave.sett(t-dt);
            FunctionAzimuthalSymmetry shockwave(&radialShockwave, 1,0,0, false);
            mSectionGraph.addFunction(&shockwave, "Analytic", Styles::GetColorScheme()->funcPlotStyles[1]);

            panel->draw();
        }

        IntPair getWindowSizeHint() override {
            return {3200, 1350};
        }


    };

}


#endif //GROWING_HOLE_GL_OUTPUT