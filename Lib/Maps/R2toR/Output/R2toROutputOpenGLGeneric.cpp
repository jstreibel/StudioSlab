//

#include "R2toROutputOpenGLGeneric.h"

#define ADD_TO_NEW_COLUMN true

R2toR::OutputOpenGL::OutputOpenGL(const NumericConfig &params, Real phiMin, Real phiMax)
: Core::Graphics::OpenGLMonitor(params, "R2 -> R OpenGL output", 1)
, phiMin(phiMin), phiMax(phiMax)
, mSectionGraph(params.getxMin(),
                params.getxMax(),
                phiMin,
                phiMax,
                "Sections",
                true,
                params.getN()*3)
, mFieldDisplay()
{
    panel.addWindow(&mSectionGraph, ADD_TO_NEW_COLUMN, .50);

    auto yMin = params.getxMin(),
         yMax = params.getxMax();

    auto line = new RtoR2::StraightLine({0, yMin},{0, yMax}, yMin, yMax);
    mSectionGraph.addSection(line, Styles::Color(1,0,0,1));

    panel.addWindow(&mFieldDisplay, ADD_TO_NEW_COLUMN, .25);
}

void R2toR::OutputOpenGL::draw() {
    Core::Graphics::OpenGLMonitor::draw();

    const R2toR::EquationState& fState = *lastData.getEqStateData<R2toR::EquationState>();
    auto &phi = fState.getPhi();

    mSectionGraph.clearFunctions();
    mSectionGraph.addFunction(&phi);

    if(mFieldDisplay.getFunction() == nullptr)
        mFieldDisplay.setFunction(DummyPtr(phi));
}



