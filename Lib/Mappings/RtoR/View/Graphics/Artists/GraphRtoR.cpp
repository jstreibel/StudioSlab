//
// Created by joao on 9/22/22.
//

#include "GraphRtoR.h"

GraphRtoR::GraphRtoR(Real xMin, Real xMax, Real yMin, Real yMax, Str title, bool filled,
                     int samples) : FunctionGraph(xMin, xMax, yMin, yMax, title,
                                                  filled, samples) {}

void GraphRtoR::_renderFunction(const RtoR::Function *func, Styles::PlotStyle style) {
      RtoR::FunctionRenderer::renderFunction(*func, style.lineColor, filled, xMin, xMax, samples);
}

