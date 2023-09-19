//
// Created by joao on 9/22/22.
//

#include "GraphRtoR.h"

GraphRtoR::GraphRtoR(Real xMin, Real xMax, Real yMin, Real yMax, Str title, bool filled,
                     int samples) : FunctionGraph(xMin, xMax, yMin, yMax, title,
                                                  filled, samples) {}

void GraphRtoR::_renderFunction(const RtoR::Function *func, Styles::PlotStyle style) {
      glLineWidth(style.thickness);
      RtoR::FunctionRenderer::renderFunction(*func, style.lineColor, style.filled, get_xMin(), get_xMax(), samples, scale);
}

void GraphRtoR::setScale(Real s) { scale = s; }

