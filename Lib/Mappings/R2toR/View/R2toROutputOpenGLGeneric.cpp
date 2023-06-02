//

#include "R2toROutputOpenGLGeneric.h"
//#include "Mappings/R2toR/Model/Transform.h"
//#include "Mappings/R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.h"
//#include "Mappings/R2toR/Model/FunctionsCollection/FunctionAzimuthalSymmetry.h"
#include "Mappings/R2toR/Model/FieldState.h"
#include "Phys/Numerics/Allocator.h"


R2toR::OutputOpenGL::OutputOpenGL(Real xMin, Real xMax, Real yMin, Real yMax, Real phiMin, Real phiMax)
    : Graphics::OutputOpenGL("R2 -> R OpenGL output", 1), xMin(xMin), xMax(xMax), yMin(yMin), yMax(yMax),
      phiMin(phiMin), phiMax(phiMax), panel(new WindowPanel),
      mSectionGraph(xMin, xMax, phiMin, phiMax, "", true,
                    Numerics::Allocator::getInstance().getNumericParams().getN()*3)
{
    Window *window = nullptr;

    //window = new Window;
    //window->addArtist(&mPhiGraph);
    //panel->addWindow(window);

    panel->addWindow(&stats);
    panel->addWindow(&mSectionGraph, true, 0.80);

    auto line = new RtoR2::StraightLine({0, yMin},{0, yMax}, yMin, yMax);
    mSectionGraph.addSection(line, Styles::Color(1,0,0,1));

}

void R2toR::OutputOpenGL::draw() {
    if(!lastData.hasValidData()) return;

    stats.addVolatileStat("O hai");

    std::stringstream ss;
    const Real t0 = 0;
    const Real L = Numerics::Allocator::getInstance().getNumericParams().getL();
    const Real xMin = Numerics::Allocator::getInstance().getNumericParams().getxLeft();

    stats.addVolatileStat(std::string("t = ") + std::to_string(getLastSimTime()));
    stats.addVolatileStat(std::string("L = ") + std::to_string(L));
    stats.addVolatileStat(std::string("xMin = ") + std::to_string(xMin));

    const R2toR::FieldState& fState = *lastData.getFieldData<R2toR::FieldState>();
    auto &phi = fState.getPhi();
    auto &dPhi = fState.getDPhiDt();

    //glDisable(GL_LINE_STIPPLE);


    // *************************************************************************************************
    // ********** SECOES *******************************************************************************

    // *************************************************************************************************
    // ********** CAMPO INTEIRO ************************************************************************
    //glEnable(GL_DEPTH_TEST);
    //phiGraph.BindWindow();
    //glMatrixMode(GL_MODELVIEW);
    //glLoadMatrixf(zpr.getModelview());

    mPhiGraph.clearFunctions();
    mPhiGraph.addFunction(&phi);

    mSectionGraph.clearFunctions();
    mSectionGraph.addFunction(&phi);


    //RtoRMap::FunctionRenderer::renderFunction(phi, xMin, xMin, xMax - xMin, phi.getN());
    //RtoR::FunctionRenderer::renderFunction(phi, xMin, xMin, xMax - xMin, 100, 100);
    //zpr.popModelViewMatrix();
    // Secoes sobre o campo inteiro

    /*
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0xAAAA);
    glColor4d(.0, .0, .0, .5);
    glLineWidth(5);
    glBegin(GL_LINES);
    {
        glVertex2d(-2, 0);
        glVertex2d(2, 0);
    }
    glEnd();

    glBegin(GL_POINTS);
    {
        glVertex2d(.0, .0);
    }
    glEnd();
    glDisable(GL_LINE_STIPPLE);
    */

    //dPhiGraph.BindWindow();
    //glMatrixMode(GL_MODELVIEW);
    //glLoadMatrixf(zpr.getModelview());
    //RtoR::FunctionRenderer::renderFunction(dPhi, xMin, xMin, xMax - xMin, 100, 100);

    panel->draw();
}

void R2toR::OutputOpenGL::notifyScreenReshape(int width, int height) {
    Base::EventListener::notifyScreenReshape(width, height);
    //ModelBase::OutputOpenGL::reshape(width, height);

    //const Real minSize = std::min(Real(windowWidth-statsWindowWidth), Real(windowHeight));
    //phiGraph =     Graph(statsWindowWidth, height/2, minSize, minSize/2, 1.1*xMin, 1.1*xMax, 1.1*yMin, 1.1*yMax);
    //dPhiGraph =    Graph(statsWindowWidth, 0,        minSize, minSize/2, 1.1*xMin, 1.1*xMax, 1.1*yMin, 1.1*yMax);
    //zpr.zprReshape(phiGraph.winX, phiGraph.winY, phiGraph.winW, phiGraph.winH);

    panel->notifyReshape(width, height);
}

void R2toR::OutputOpenGL::notifyKeyboardSpecial(int key, int x, int y) {
}

void R2toR::OutputOpenGL::notifyKeyboard(unsigned char key, int x, int y) {
    if(key == '2'){
        showAnalytic = !showAnalytic;
        return;
    }
    else if(key == ']'){
        yMin *= 1.1;
        yMax *= 1.1;
    }
    else if(key == '['){
        yMin /= 1.1;
        yMax /= 1.1;
    }
    else if(key == '}'){
        yMin *= 1.5;
        yMax *= 1.5;
    }
    else if(key == '{'){
        yMin /= 1.5;
        yMax /= 1.5;
    }
}

IntPair R2toR::OutputOpenGL::getWindowSizeHint() {
    return {3200, 1350};
}

void R2toR::OutputOpenGL::notifyMouseButton(int button, int dir, int x, int y) {
    //zpr.zprMouseButton(button, dir, x, y);
}

void R2toR::OutputOpenGL::notifyMouseMotion(int x, int y) {
    //zpr.zprMouseMotion(x, y);
}
