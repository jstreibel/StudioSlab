//

#include "RtoROutputOpenGL.h"

#include "../../Model/FunctionsCollection/AnalyticShockwave1D.h"

#include <Phys/DifferentialEquations/2nd-Order/Lorentz-2ndOrder.h>

#include "../../../FunctionRenderer.h"
#include "Fields/Mappings/RtoR/Model/FunctionsCollection/IntegerPowerFunctions.h"
//
// Created by joao on 23/09/2019.


#define max(a, b) ((a)>(b)?(a):(b))
#define min(a, b) ((a)<(b)?(a):(b))


RtoR::OutputOpenGL::OutputOpenGL() : panel(new WindowPanel) { }

RtoR::OutputOpenGL::OutputOpenGL(const double xMin, const double xMax,
                                 const double phiMin, const double phiMax)
                                 : panel(new WindowPanel), mFieldsGraph(xMin, xMax, phiMin, phiMax, "Fields")
{
    initialize(xMin, xMax, phiMin, phiMax);
}

void RtoR::OutputOpenGL::initialize(double xMin, double xMax, double phiMin, double phiMax) {
    if(isInitialized) return;

    {
        this->xMin = xMin;
        this->xMax = xMax;

        this->phiMin = phiMin;
        this->phiMax = phiMax;
    }


    Window *window = nullptr;


    //this->mTotalEnergyGraph = {0, Allocator::getInstance().getNumericParams().gett(), 0, 1,
    //                           "U, K, W, |phi|, W+|phi|", true};
    //this->mTotalEnergyGraph.addFunction(&UHistory, U_color);
    //this->mTotalEnergyGraph.addFunction(&KHistory, K_color);
    //this->mTotalEnergyGraph.addFunction(&WHistory, W_color);
    //this->mTotalEnergyGraph.addFunction(&VHistory, V_color);

    //window = new Window; window->addArtist(&this->mTotalEnergyGraph);
    //panel->addWindow(window);


    window = new Window; window->addArtist(&this->stats);
    panel->addWindow(window);

    auto samples = (int)Numerics::Allocator::getInstance().getNumericParams().getN();
    this->mFieldsGraph = {xMin, xMax, -0.1*phiMax, phiMax, "AAA", true, samples};
    window = new Window; window->addArtist(&this->mFieldsGraph);
    panel->addWindow(window, true, 0.7);



    const auto faktor = 2000;
    phiMinAnim = new Animation(&mFieldsGraph.yMin, mFieldsGraph.yMin, faktor);
    phiMaxAnim = new Animation(&mFieldsGraph.yMax, mFieldsGraph.yMax, faktor);
    addAnimation(phiMaxAnim);
    addAnimation(phiMinAnim);

    xMinAnim = new Animation(&mFieldsGraph.xMin, mFieldsGraph.xMin, faktor);
    xMaxAnim = new Animation(&mFieldsGraph.xMax, mFieldsGraph.xMax, faktor);
    addAnimation(xMinAnim);
    addAnimation(xMaxAnim);

    //UHistory.xMin = 0;
    //UHistory.xMax = Allocator::getInstance().getNumericParams().gett();
    //KHistory.xMin = 0;
    //KHistory.xMax = Allocator::getInstance().getNumericParams().gett();
    //WHistory.xMin = 0;
    //WHistory.xMax = Allocator::getInstance().getNumericParams().gett();
    //VHistory.xMin = 0;
    //VHistory.xMax = Allocator::getInstance().getNumericParams().gett();


    isInitialized = true;
}

void RtoR::OutputOpenGL::draw() {
    if(!isInitialized) {
        const auto &p = Numerics::Allocator::getInstance().getNumericParams();
        auto xMin = p.getxLeft();
        auto xMax = xMin + p.getL();

        initialize(xMin, xMax, -0.08, 0.08);
    }


    stats.addVolatileStat("");
    stats.addVolatileStat(std::string("t = ") + std::to_string(getLastSimTime()));
    static size_t lastStep = 0;
    stats.addVolatileStat(std::string("step ") + std::to_string(lastInfo.getSteps()));
    stats.addVolatileStat(std::string("delta step: ") + std::to_string(lastInfo.getSteps() - lastStep));
    lastStep = lastInfo.getSteps();



    // *************************** FIELD ***********************************
    //mFieldsGraph.draw();
    const RtoR::FieldState &fieldState = *lastInfo.getFieldData<RtoR::FieldState>();
    if(&fieldState == nullptr) throw "Fieldstate data doesn't seem to be RtoRMap.";

    mFieldsGraph.clearFunctions();

    if(showPhi){
        const Color colorPhi = V_color;

        mFieldsGraph.addFunction(&energyCalculator.getPotential(), colorPhi, "|phi|");
    }

    if(showKineticEnergy){
        const Color colorKinetic = K_color;

        // mFieldsGraph.addFunction(&energyCalculator.getKinetic(), colorKinetic);
        mFieldsGraph.addFunction(&fieldState.getDPhiDt(), colorKinetic, "kinetic");
    }

    if(showGradientEnergy) {
        const Color colorGradient = W_color;

        mFieldsGraph.addFunction(&energyCalculator.getGradient(), colorGradient, "grad^2");
    }

    if(showEnergyDensity){
        const Color color = U_color;
        mFieldsGraph.addFunction(&energyCalculator.getEnergy(), color, "E");
    }

    panel->draw(true, true);

}

void RtoR::OutputOpenGL::_out(const OutputPacket &outInfo) {

    const RtoR::FieldState &fieldState = *outInfo.getFieldData<RtoR::FieldState>();
    energyCalculator.computeDensities(fieldState);

    //auto U = energyCalculator.integrateEnergy();
    //auto K = energyCalculator.integrateKinetic();
    //auto W = energyCalculator.integrateGradient();
    //auto V = energyCalculator.integratePotential();
//
    //energyTotal = U;
//
    //if(UHistory.X.size() == 0) {
    //    // Isso eh necessario pra evitar bugs estranhos. Sorry.
    //    const double k = 1-1.e-5;
    //    UHistory.insertBack(U * k);
    //    UHistory.insertBack(U / k);
    //}
//
    //UHistory.insertBack(U);
    //KHistory.insertBack(K);
    //WHistory.insertBack(W);
    //VHistory.insertBack(V);
//
    //auto xMax = outInfo.getT();
//
    //auto yMin = min(UHistory.getYMin(), min(KHistory.getYMin(), min(WHistory.getYMin(), VHistory.getYMin())));
    //auto yMax = max(UHistory.getYMax(), min(KHistory.getYMax(), min(WHistory.getYMax(), VHistory.getYMax())));
//
    //mTotalEnergyGraph.yMax = yMax;
    //mTotalEnergyGraph.yMin = yMin;
//
    //mTotalEnergyGraph.xMax = xMax;
//
    //UHistory.xMax = xMax;
    //KHistory.xMax = xMax;
    //WHistory.xMax = xMax;
    //VHistory.xMax = xMax;

    Base::OutputOpenGL::_out(outInfo);
}


void RtoR::OutputOpenGL::notifyReshape(int width, int height) {
    windowWidth = width;
    windowHeight = height;

    panel->reshape(width, height);
    //panel->w = width;
    //panel->h = height;
//
    //panel->arrangeWindows();
}

void RtoR::OutputOpenGL::notifyKeyboard(unsigned char key, int x, int y) {

    switch(key)
    {
        case '1':
            showPhi = !showPhi;
            break;
        case '2':
            showKineticEnergy = !showKineticEnergy;
            break;
        case '3':
            showGradientEnergy = !showGradientEnergy;
            break;
        case '4':
            showEnergyDensity = !showEnergyDensity;
            std::cout << "Important: energy density is computing with the signum potential only." << std::endl;
            break;
        default:
            break;
    }

    finishFrameAndRender();
}

void RtoR::OutputOpenGL::notifyKeyboardSpecial(int key, int x, int y) {
    const double kDown = 3.8;
    const double kUp = 1/kDown;

    const auto delta = (mFieldsGraph.xMax - mFieldsGraph.xMin);
    const auto center = .5*(mFieldsGraph.xMax + mFieldsGraph.xMin);
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
            xMaxAnim->animateToValue(mFieldsGraph.xMax-k*delta);
            xMinAnim->animateToValue(mFieldsGraph.xMin-k*delta);
            break;
        case GLUT_KEY_RIGHT:
            xMaxAnim->animateToValue(mFieldsGraph.xMax+k*delta);
            xMinAnim->animateToValue(mFieldsGraph.xMin+k*delta);
            break;

        default:
            break;
    }

    finishFrameAndRender();
}

void RtoR::OutputOpenGL::notifyMouseButton(int button, int dir, int x, int y) {
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




