//
// Created by joao on 9/22/22.
//

#include "GraphR2ToR.h"
#include "Fields/Mappings/FunctionRenderer.h"

GraphR2toR::GraphR2toR(double xMin, double xMax, double yMin, double yMax, String title, bool filled,
                       int samples) : Graph(xMin, xMax, yMin, yMax, title, filled, samples) {

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

void GraphR2toR::reshape(int w, int h) {
    Artist::reshape(w, h);

    zpr.zprReshape(0, 0, 1800, 1200);
}

