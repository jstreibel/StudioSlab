//
// Created by joao on 9/22/22.
//

#include "GraphR2ToR.h"
#include "Math/Function/MapsFunctionRenderer.h"

GraphR2toR::GraphR2toR(DevFloat xMin, DevFloat xMax, DevFloat yMin, DevFloat yMax, Str title, bool filled,
                       int samples) : FunctionGraph(xMin, xMax, yMin, yMax, title, filled, samples) {
    GUIEventListener::addResponder(&zpr);
}

void GraphR2toR::_renderFunction(const R2toR::Function *func, PlotStyle style) {
    glEnable(GL_DEPTH_TEST);
    //phiGraph.BindWindow();
    glMatrixMode(GL_MODELVIEW);

    glLoadMatrixf(zpr.getModelview());

    //RtoR::FunctionRenderer::renderFunction(phi, xMin, xMin, xMax - xMin, phi.getN());
    RtoR::FunctionRenderer::renderFunction(*func, get_xMin(), get_xMin(), getRegion().width(), 100, 100);
    //zpr.popModelViewMatrix();
}

void GraphR2toR::notifyReshape(int newWinW, int newWinH) {
    Window::notifyReshape(newWinW, newWinH);

    zpr.zprReshape(getx(), gety(), getw(), geth());
}

