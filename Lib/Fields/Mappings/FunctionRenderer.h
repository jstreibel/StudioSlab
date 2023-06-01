//
// Created by joao on 27/09/2019.
//

#ifndef V_SHAPE_FUNCTIONRENDERER_H
#define V_SHAPE_FUNCTIONRENDERER_H

// TODO these #includes below shouldn't be THIS far from this file
#include "RtoR/Model/RtoRFunction.h"
#include "RtoR/Model/RtoRFunctionArbitrary.h"
#include "R2toR/Model/R2toRFunction.h"
#include "RtoR2/StraightLine.h"

#include "Base/Graphics/PlottingUtils.h"
#include "Base/Graphics/Styles/StylesAndColorSchemes.h"


namespace RtoR {

    class FunctionRenderer {
    public:
        static void renderFunction(          const RtoR::ArbitraryFunction &func, Styles::Color c, bool filled,
                                             Real scale = 1.0);
        static void renderFunction(          const RtoR::Function &func, Styles::Color c, bool filled,
                                             Real xMin, Real xMax, PosInt resolution, Real scale = 1.0);
        static void renderFunction(          const R2toR::Function &func, Real xMin, Real yMin,
                                             Real L, PosInt nLines, PosInt linesRes, Real scale = 1.0);


        static void renderSection(           const R2toR::Function &func, const RtoR2::StraightLine &section,
                                             Styles::PlotStyle style, PosInt resolution, Real scale=1.0);
        static void renderHorizontalSection( const R2toR::Function &func, Styles::Color c, bool filled,
                                             Real xMin, Real xMax, PosInt resolution);
        static void renderVerticalSection(   const R2toR::Function &func, Styles::Color c, bool filled,
                                             Real yMin, Real yMax, PosInt resolution);


    };

}

#endif //V_SHAPE_FUNCTIONRENDERER_H
