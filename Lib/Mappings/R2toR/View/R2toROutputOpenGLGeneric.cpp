//

#include "R2toROutputOpenGLGeneric.h"
//#include "Mappings/R2toR/Model/Transform.h"
//#include "Mappings/R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.h"
//#include "Mappings/R2toR/Model/FunctionsCollection/FunctionAzimuthalSymmetry.h"
#include "Mappings/R2toR/Model/EquationState.h"
#include "Phys/Numerics/Allocator.h"


R2toR::OutputOpenGL::OutputOpenGL(const NumericParams &params, Real xMin, Real xMax, Real yMin, Real yMax, Real phiMin, Real phiMax)
    : Graphics::OutputOpenGL(params, "R2 -> R OpenGL output", 1)
    , eqState(*Numerics::Allocator::NewFieldState<R2toR::EquationState>())
    , xMin(xMin), xMax(xMax), yMin(yMin), yMax(yMax)
    , phiMin(phiMin), phiMax(phiMax)
    , mSectionGraph(xMin, xMax, phiMin, phiMax,
                    "Sections", true,
                    Numerics::Allocator::GetInstance().getNumericParams().getN())
{
    // Window *window = nullptr;

    //window = new Window;
    //window->addArtist(&mPhiGraph);
    //panel->addWindow(window);

    panel.addWindow(&stats);
    panel.addWindow(&mSectionGraph, true, 0.80);

    auto line = new RtoR2::StraightLine({0, yMin},{0, yMax}, yMin, yMax);
    mSectionGraph.addSection(line, Styles::Color(1,0,0,1));

}

R2toR::OutputOpenGL::~OutputOpenGL() {

}

void R2toR::OutputOpenGL::draw() {
    if(!lastData.hasValidData()) return;

    stats.addVolatileStat("O hai");

    std::stringstream ss;
    const Real t0 = 0;
    const Real L = Numerics::Allocator::GetInstance().getNumericParams().getL();
    const Real xMin = Numerics::Allocator::GetInstance().getNumericParams().getxLeft();

    stats.addVolatileStat(std::string("t = ") + std::to_string(getLastSimTime()));
    stats.addVolatileStat(std::string("L = ") + std::to_string(L));
    stats.addVolatileStat(std::string("xMin = ") + std::to_string(xMin));

    const R2toR::EquationState& fState = *lastData.getEqStateData<R2toR::EquationState>();
    auto &phi = fState.getPhi();
    auto &dPhi = fState.getDPhiDt();

    mSectionGraph.clearFunctions();
    mSectionGraph.addFunction(&phi);

    panel.draw();
}

bool R2toR::OutputOpenGL::notifyKeyboard(unsigned char key, int x, int y) {
    if(key == '2'){
        showAnalytic = !showAnalytic;
        return true;
    }
    else if(key == ']'){
        yMin *= 1.1;
        yMax *= 1.1;
        return true;
    }
    else if(key == '['){
        yMin /= 1.1;
        yMax /= 1.1;
        return true;
    }
    else if(key == '}'){
        yMin *= 1.5;
        yMax *= 1.5;
        return true;
    }
    else if(key == '{'){
        yMin /= 1.5;
        yMax /= 1.5;
        return true;
    }

    return Graphics::OutputOpenGL::notifyKeyboard(key, x, y);
}

void R2toR::OutputOpenGL::_out(const OutputPacket &outInfo) {
    Graphics::OutputOpenGL::_out(outInfo);

    auto &state = *outInfo.getEqStateData<R2toR::EquationState>();

    eqState.setPhi(     state.getPhi());
    eqState.setDPhiDt(  state.getDPhiDt());

}

