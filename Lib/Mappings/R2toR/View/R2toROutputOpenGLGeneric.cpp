//

#include "R2toROutputOpenGLGeneric.h"
#include "Mappings/R2toR/Model/EquationState.h"


R2toR::OutputOpenGL::OutputOpenGL(const NumericParams &params, Real phiMin, Real phiMax)
    : Graphics::Monitor(params, "R2 -> R OpenGL output", 1)
    , phiMin(phiMin), phiMax(phiMax)
    , mSectionGraph(params.getxMin(), params.getxMax(), phiMin, phiMax,
                    "Sections", true,
                    params.getN()*3)
{
    // Window *window = nullptr;

    //window = new Window;
    //window->addArtist(&mPhiGraph);
    //panel->addWindow(window);

    panel.addWindow(&stats);
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
    Graphics::Monitor::draw();

    const R2toR::EquationState& fState = *lastData.getEqStateData<R2toR::EquationState>();
    auto &phi = fState.getPhi();

    mSectionGraph.clearFunctions();
    mSectionGraph.addFunction(&phi);

    panel.draw();
}



