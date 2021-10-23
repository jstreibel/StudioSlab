//

#include <Lib/Fields/Maps/R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.h>
#include <Model/SystemGordon.h>
#include <Studios/MathTools/Maps/RtoR/Model/RtoRFunctionArbitraryCPU.h>
#include <Studios/MathTools/Maps/RtoR/Model/RtoRFuncArbResizable.h>
#include <Studios/MathTools/Maps/RtoR/Model/FunctionsCollection/AnalyticShockwave1D.h>
#include "RtoROutputOpenGL.h"
#include <Studios/Graphics/FunctionRenderer.h>
//
// Created by joao on 23/09/2019.




RtoR::OutputOpenGL::OutputOpenGL(const double xMin, const double xMax,
                                 const double phiMin, const double phiMax)
{
    initialize(xMin, xMax, phiMin, phiMax);
}

void RtoR::OutputOpenGL::initialize(double xMin, double xMax, double phiMin, double phiMax) {
    if(isInitialized) return;

    isInitialized = true;

    this->xMin = xMin;
    this->xMax = xMax;

    this->phiMin = phiMin;
    this->phiMax = phiMax;


    this->fieldsGraph = {statsWindowWidth, 0, double(windowWidth)-statsWindowWidth, double(windowHeight), xMin, xMax, phiMin, phiMax};
    auto H = (double(windowHeight)-statsWindowHeight);
    this->totalEnergyGraph = {0, statsWindowHeight, statsWindowWidth, H, 0,
                              Allocator::getInstance().getNumericParams().gett(), 0, 1, "E(t))"};


    phiMinAnim = new Animation(&fieldsGraph.yMin, fieldsGraph.yMin, 120);
    phiMaxAnim = new Animation(&fieldsGraph.yMax, fieldsGraph.yMax, 120);
    addAnimation(phiMaxAnim);
    addAnimation(phiMinAnim);

    xMinAnim = new Animation(&fieldsGraph.xMin, fieldsGraph.xMin, 120);
    xMaxAnim = new Animation(&fieldsGraph.xMax, fieldsGraph.xMax, 120);
    addAnimation(xMinAnim);
    addAnimation(xMaxAnim);

    energyHistory.xMin = 0;
    energyHistory.xMax = Allocator::getInstance().getNumericParams().gett();

}

void RtoR::OutputOpenGL::draw() {
    if(!isInitialized) {
        const auto &p = Allocator::getInstance().getNumericParams();
        auto xMin = p.getxLeft();
        auto xMax = xMin + p.getL();

        initialize(xMin, xMax, -0.08, 0.08);
    }


    // *************************** ENERGY *********************************
    totalEnergyGraph.DrawAxes();
    FunctionRenderer::renderFunction(energyHistory, Color(.9f, .5f, .0f, .9),
                                     false, energyHistory.xMin, energyHistory.xMax, energyHistory.X.size());

    addVolatileStat(std::string("E: ") + std::to_string(energyTotal));
    addVolatileStat(std::string("E.size(): ") + std::to_string(energyHistory.X.size()));


    // *************************** FIELD ***********************************
    fieldsGraph.DrawAxes();
    const RtoR::FieldState &fieldState = *lastInfo.getFieldData<RtoR::FieldState>();
    if(&fieldState == nullptr) throw "Fieldstate data doesn't seem to be RtoRMap.";

    if(showPhi){
        const Color colorPhi = Color(.5f, .5f, 1.0f);
        FunctionRenderer::renderFunction(fieldState.getPhi(), colorPhi, true);
    }

    if(showDPhiDt){
        const Color colorDPhiDt = Color(.4f, .4f, .2f, .5f);
        FunctionRenderer::renderFunction(fieldState.getDPhiDt(), colorDPhiDt, true);
    }

    if(showEnergyDensity){
        const Color color = Color(.9f, .5f, .0f, .9);
        FunctionRenderer::renderFunction(energyCalculator.getFunc(), color,true);
    }

    // light cone
    {
        const Real t = getLastT();
        glColor4f(.2, .4, .8, .15);
        glBegin(GL_LINES);
        glVertex2f(t, getPhiMin());
        glVertex2f(t, getPhiMax());

        glVertex2f(-t, getPhiMin());
        glVertex2f(-t, getPhiMax());
        glEnd();
    }


    // *************************** PHASE SPACE *****************************
    phaseSpaceGraph.DrawAxes();

    auto &Q = fieldState.getPhi().getSpace().getX(),
         &P = fieldState.getDPhiDt().getSpace().getX();

    #define sign(x) ((x)<0?-1.:1.)
    auto rq = .0;
    auto rp = .0;
    for(auto &q : Q) rq+=q;
    for(auto &p : P) rp+=p;
    rq = (rq/Q.size());
    rp = (rp/P.size());

    static std::vector<std::pair<double, double>> points;
    points.emplace_back(rq, rp);

    glPointSize(2);
    glColor4f(1,1,1,0.4f);
    glBegin(GL_POINTS);
    for(auto &point : points) glVertex2f(point.first, point.second);
    glEnd();

}

void RtoR::OutputOpenGL::_out(const OutputPacket &outInfo) {

    const RtoR::FieldState &fieldState = *outInfo.getFieldData<RtoR::FieldState>();
    energyCalculator.computeEnergyDensity(fieldState);
    energyTotal = energyCalculator.integrate();

    if(energyHistory.X.size() == 0) {
        // Isso eh necessario pra evitar bugs estranhos. Sorry.
        const double k = 1-1.e-5;
        energyHistory.insertBack(energyTotal*k);
        energyHistory.insertBack(energyTotal/k);
    }
    energyHistory.insertBack(energyTotal);
    totalEnergyGraph.yMax = energyHistory.getYMax();
    totalEnergyGraph.yMin = energyHistory.getYMin();
    totalEnergyGraph.xMax = outInfo.getT();
    energyHistory.xMax = totalEnergyGraph.xMax;

    Base::OutputOpenGL::_out(outInfo);
}


void RtoR::OutputOpenGL::reshape(int width, int height) {
    windowWidth = width;
    windowHeight = height;

    fieldsGraph = Graph(statsWindowWidth, 0, windowWidth-statsWindowWidth, windowHeight,
            1.1*xMin, 1.1*xMax, phiMin, phiMax);

    auto Y = statsWindowHeight;
    auto H = (windowHeight-statsWindowHeight)*.5;

    totalEnergyGraph = Graph(0, Y, statsWindowWidth, H,
                             0, 3, 0, 3.e5, "E(t))");

    Y+=H;
    phaseSpaceGraph = Graph(0, Y, statsWindowWidth, H,
                            -0.015, 0.015, -0.080, 0.080, "phase space");
}

void RtoR::OutputOpenGL::notifyGLUTKeyboard(unsigned char key, int x, int y) {

    switch(key)
    {
        case '1':
            showPhi = !showPhi;
            break;
        case '2':
            showDPhiDt = !showDPhiDt;
            break;
        case '3':
            showEnergyDensity = !showEnergyDensity;
            std::cout << "Important: energy density is computing with the signum potential only." << std::endl;
            break;
        case '4':
            showAnalyticSolution = !showAnalyticSolution;
            break;
        default:
            break;
    }

    finishFrameAndRender();
}

void RtoR::OutputOpenGL::notifyGLUTKeyboardSpecial(int key, int x, int y) {
    const double kDown = 3.8;
    const double kUp = 1/kDown;

    const auto delta = (fieldsGraph.xMax - fieldsGraph.xMin);
    const auto center = .5*(fieldsGraph.xMax + fieldsGraph.xMin);
    const auto k = .2;


    switch(key) {
        case GLUT_KEY_PAGE_UP:
            setPhiMax(getPhiMax() * kUp);
            setPhiMin(getPhiMin() * kUp);
            break;
        case GLUT_KEY_PAGE_DOWN:
            setPhiMax(getPhiMax() * kDown);
            setPhiMin(getPhiMin() * kDown);
            break;

        case GLUT_KEY_UP:
            xMaxAnim->animateToValue(center+k*delta);
            xMinAnim->animateToValue(center-k*delta);
            break;
        case GLUT_KEY_DOWN:
            xMaxAnim->animateToValue(center+(1.+k)*delta);
            xMinAnim->animateToValue(center-(1.+k)*delta);
            break;
        case GLUT_KEY_LEFT:
            xMaxAnim->animateToValue(fieldsGraph.xMax-k*delta);
            xMinAnim->animateToValue(fieldsGraph.xMin-k*delta);
            break;
        case GLUT_KEY_RIGHT:
            xMaxAnim->animateToValue(fieldsGraph.xMax+k*delta);
            xMinAnim->animateToValue(fieldsGraph.xMin+k*delta);
            break;

        default:
            break;
    }

    finishFrameAndRender();
}

void RtoR::OutputOpenGL::notifyGLUTMouseButton(int button, int dir, int x, int y) {
    const double faktor = 1.5;
    if(button == 3 && dir == 0){
        setPhiMax(getPhiMax()*faktor);
        setPhiMin(getPhiMin()*faktor);
    }
    else if(button == 4 && dir == 0){
        setPhiMax(getPhiMax()*(1/faktor));
        setPhiMin(getPhiMin()*(1/faktor));
    }

    finishFrameAndRender();
}


