//
// Created by joao on 9/22/22.
//

#include "GraphR2ToR.h"
#include "Maps/FunctionRenderer.h"

GraphR2toR::GraphR2toR(Real xMin, Real xMax, Real yMin, Real yMax, Str title, bool filled,
                       int samples) : FunctionGraph(xMin, xMax, yMin, yMax, title, filled, samples) {
    EventListener::addResponder(&zpr);
}

void GraphR2toR::_renderFunction(const R2toR::Function *func, Styles::PlotStyle style) {
    glEnable(GL_DEPTH_TEST);
    //phiGraph.BindWindow();
    glMatrixMode(GL_MODELVIEW);

    glLoadMatrixf(zpr.getModelview());

    //RtoR::FunctionRenderer::renderFunction(phi, xMin, xMin, xMax - xMin, phi.getN());
    RtoR::FunctionRenderer::renderFunction(*func, xMin, xMin, xMax - xMin, 100, 100);
    //zpr.popModelViewMatrix();
}

void GraphR2toR::notifyReshape(int newWinW, int newWinH) {
    Window::notifyReshape(newWinW, newWinH);

    zpr.zprReshape(x, y, w, h);
}

