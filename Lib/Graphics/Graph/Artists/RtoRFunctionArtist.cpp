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

    void RtoRFunctionArtist::draw(const PlottingWindow &d) {
        if(function == nullptr) return;

        glLineWidth(plotStyle.thickness);

        Graphics::FunctionRenderer::renderFunction(*function, plotStyle.lineColor, plotStyle.filled, d.get_xMin(),
                                                   d.get_xMax(), samples, 1);
    }
} // Graphics