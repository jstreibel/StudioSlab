//
// Created by joao on 27/09/2019.
//

#ifndef V_SHAPE_FUNCTIONRENDERER_H
#define V_SHAPE_FUNCTIONRENDERER_H

#include "Graphics/Plot2D/PlotStyle.h"
#include "Graphics/Utils.h"

// TODO these #includes below shouldn't be THIS far from this file
#include "Math/Function/RtoR/Model/RtoRFunction.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunction.h"
#include "Math/Function/R2toR/Model/R2toRFunction.h"
#include "Math/Function/RtoR2/StraightLine.h"


namespace Slab::Graphics {

    using namespace Math;

    class FunctionRenderer {
    public:

        static void renderFunction(          const RtoR::NumericFunction &func, Color c, bool filled,
                                             Real scale = 1.0);
        static void renderFunction(          const RtoR::Function &func, Color c, bool filled,
                                             Real xMin, Real xMax, UInt resolution, Real scale = 1.0);
        static void renderFunction(          const R2toR::Function &func, Real xMin, Real yMin,
                                             Real L, UInt nLines, UInt linesRes, Real scale = 1.0);


        static void renderSection(           const R2toR::Function &func, const RtoR2::ParametricCurve &section,
                                             PlotStyle style, UInt resolution, Real scale=1.0);
        static void renderHorizontalSection( const R2toR::Function &func, Color c, bool filled,
                                             Real xMin, Real xMax, UInt resolution);
        static void renderVerticalSection(   const R2toR::Function &func, Color c, bool filled,
                                             Real yMin, Real yMax, UInt resolution);


    };

}

#endif //V_SHAPE_FUNCTIONRENDERER_H
