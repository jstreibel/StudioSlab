//
// Created by joao on 21/05/24.
//

#include "RtoRFunctionArtist.h"
#include "Graphics/OpenGL/LegacyGL/FunctionRenderer.h"
#include "Graphics/Graph/PlottingWindow.h"

#include <utility>

namespace Slab::Graphics {
    RtoRFunctionArtist::RtoRFunctionArtist(RtoR::Function_ptr func, PlotStyle plotStyle, Count samples)
    : function(std::move(func)), plotStyle(plotStyle), samples(samples)
    {    }

    bool RtoRFunctionArtist::draw(const PlottingWindow &d) {
        if(function == nullptr) return true;

        glLineWidth(plotStyle.thickness);

        auto graphRect = d.getRegion().getRect();
        Graphics::FunctionRenderer::renderFunction(*function, plotStyle.lineColor, plotStyle.filled, graphRect.xMin,
                                                   graphRect.xMax, samples, 1);

        return true;
    }
} // Graphics