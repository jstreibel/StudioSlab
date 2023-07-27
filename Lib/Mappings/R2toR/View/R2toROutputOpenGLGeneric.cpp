//

#include "R2toROutputOpenGLGeneric.h"
#include "Mappings/R2toR/Model/EquationState.h"


R2toR::OutputOpenGL::OutputOpenGL(const NumericParams &params, Real phiMin, Real phiMax)
    : Graphics::OutputOpenGL(params, "R2 -> R OpenGL output", 1)
    , phiMin(phiMin), phiMax(phiMax)
    , mSectionGraph(params.getxLeft(), params.getxMax(), phiMin, phiMax,
                    "Sections", true,
                    params.getN()*3)
{
    // Window *window = nullptr;

    //window = new Window;
    //window->addArtist(&mPhiGraph);
    //panel->addWindow(window);

    panel.addWindow(&stats);
    panel.addWindow(&mSectionGraph, true, 0.80);

    auto yMin = params.getxLeft(),
         yMax = params.getxMax();

    auto line = new RtoR2::StraightLine({0, yMin},{0, yMax}, yMin, yMax);
    mSectionGraph.addSection(line, Styles::Color(1,0,0,1));

}

R2toR::OutputOpenGL::~OutputOpenGL() {
    // delete panel;
}

void R2toR::OutputOpenGL::draw() {
    if(!lastData.hasValidData()) return;

    stats.addVolatileStat("O hai");

    std::stringstream ss;
    const Real t0 = 0;

    stats.addVolatileStat(std::string("t = ") + std::to_string(getLastSimTime()));
    //stats.addVolatileStat(std::string("L = ") + std::to_string(L));
    //stats.addVolatileStat(std::string("xMin = ") + std::to_string(xMin));

    const R2toR::EquationState& fState = *lastData.getEqStateData<R2toR::EquationState>();
    auto &phi = fState.getPhi();
    auto &dPhi = fState.getDPhiDt();

    mSectionGraph.clearFunctions();
    mSectionGraph.addFunction(&phi);

    panel.draw();
}



