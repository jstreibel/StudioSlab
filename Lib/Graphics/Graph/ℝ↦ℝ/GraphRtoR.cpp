//
// Created by joao on 9/22/22.
//

#include "GraphRtoR.h"
#include "Graphics/OpenGL/LegacyGL/FunctionRenderer.h"

namespace Graphics {

    GraphRtoR::GraphRtoR(Real xMin, Real xMax, Real yMin, Real yMax, Str title, bool filled,
                         int samples) : FunctionGraph(xMin, xMax, yMin, yMax, title,
                                                      filled, samples) {}

    void GraphRtoR::_renderFunction(const RtoR::Function *func, PlotStyle style) {
        glLineWidth(style.thickness);
        Graphics::FunctionRenderer::renderFunction(*func, style.lineColor, style.filled, get_xMin(),
                                               get_xMax(), samples, scale);
    }

    void GraphRtoR::setScale(Real s) { scale = s; }

}