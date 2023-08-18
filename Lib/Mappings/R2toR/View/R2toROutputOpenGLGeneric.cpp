//

#include "R2toROutputOpenGLGeneric.h"

R2toR::OutputOpenGL::OutputOpenGL(const NumericConfig &params, Real phiMin, Real phiMax)
    : Graphics::OpenGLMonitor(params, "R2 -> R OpenGL output", 1)
    , phiMin(phiMin), phiMax(phiMax)
    , mSectionGraph(params.getxMin(), params.getxMax(), phiMin, phiMax,
                    "Sections", true,
                    params.getN()*3)
{
    panel.addWindow(&mSectionGraph, true, 0.80);

    auto yMin = params.getxMin(),
         yMax = params.getxMax();

    auto line = new RtoR2::StraightLine({0, yMin},{0, yMax}, yMin, yMax);
    mSectionGraph.addSection(line, Styles::Color(1,0,0,1));

}

R2toR::OutputOpenGL::~OutputOpenGL() {
    // delete panel;
}

void R2toR::OutputOpenGL::draw() {
    Graphics::OpenGLMonitor::draw();

    const R2toR::EquationState& fState = *lastData.getEqStateData<R2toR::EquationState>();
    auto &phi = fState.getPhi();

    mSectionGraph.clearFunctions();
    // mSectionGraph.addFunction(&phi);
}



