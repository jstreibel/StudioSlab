//

#include "R2toROutputOpenGLGeneric.h"
//#include "Fields/Mappings/R2toR/Model/Transform.h"
//#include "Fields/Mappings/R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.h"
//#include "Fields/Mappings/R2toR/Model/FunctionsCollection/FunctionAzimuthalSymmetry.h"
#include "Fields/Mappings/R2toR/Model/FieldState.h"
#include "Phys/Numerics/Allocator.h"


R2toR::OutputOpenGL::OutputOpenGL(Real xMin, Real xMax, Real yMin, Real yMax, Real phiMin, Real phiMax)
                                  : Base::OutputOpenGL(), xMin(xMin), xMax(xMax), yMin(yMin), yMax(yMax),
                                    phiMin(phiMin), phiMax(phiMax), panel(new WindowPanel),
                                    mSectionGraph(0, 1, phiMin, phiMax, "section", true, 1024)
{
    std::cout << "Initialized R2toRMap::OutputOpenGL." << std::endl;

    Window *window = nullptr;

    window = new Window;
    //window->addArtist(&mPhiGraph);
    panel->addWindow(window);

    window = new Window;
    window->addArtist(&stats);
    panel->addWindow(window);

    window = new Window;
    window->addArtist(&mSectionGraph);
    panel->addWindow(window, true, 0.8);
    mSectionGraph.addSection(new RtoR2::StraightLine({xMin, 0},{xMax, 0}));

}

void R2toR::OutputOpenGL::draw() {
    if(!lastInfo.hasValidData()) return;

    stats.addVolatileStat("O hai");

    std::stringstream ss;
    const Real t0 = 0;
    const Real L = Allocator::getInstance().getNumericParams().getL();

    stats.addVolatileStat(std::string("t = ") + std::to_string(getLastT()));

    const R2toR::FieldState& fState = *lastInfo.getFieldData<R2toR::FieldState>();
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

    panel->draw(true, true);
}

void R2toR::OutputOpenGL::notifyReshape(int width, int height) {
    //ModelBase::OutputOpenGL::reshape(width, height);

    windowWidth = width;
    windowHeight = height;

    //const Real minSize = std::min(Real(windowWidth-statsWindowWidth), Real(windowHeight));
    //phiGraph =     Graph(statsWindowWidth, height/2, minSize, minSize/2, 1.1*xMin, 1.1*xMax, 1.1*yMin, 1.1*yMax);
    //dPhiGraph =    Graph(statsWindowWidth, 0,        minSize, minSize/2, 1.1*xMin, 1.1*xMax, 1.1*yMin, 1.1*yMax);
    //zpr.zprReshape(phiGraph.winX, phiGraph.winY, phiGraph.winW, phiGraph.winH);

    panel->reshape(width, height);
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
    return {1200, 800};
}

void R2toR::OutputOpenGL::notifyMouseButton(int button, int dir, int x, int y) {
    //zpr.zprMouseButton(button, dir, x, y);
}

void R2toR::OutputOpenGL::notifyMouseMotion(int x, int y) {
    //zpr.zprMouseMotion(x, y);
}
