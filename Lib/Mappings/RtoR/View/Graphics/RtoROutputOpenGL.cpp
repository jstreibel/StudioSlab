//

#include "RtoROutputOpenGL.h"

#include "Common/Log/Log.h"

#include "Phys/DifferentialEquations/2nd-Order/GordonSystem.h"

#include "Mappings/FunctionRenderer.h"
#include "Mappings/RtoR/Model/FunctionsCollection/AnalyticShockwave1D.h"
#include "Mappings/RtoR/Model/FunctionsCollection/IntegerPowerFunctions.h"
//
// Created by joao on 23/09/2019.


#define max(a, b) ((a)>(b)?(a):(b))
#define min(a, b) ((a)<(b)?(a):(b))


RtoR::OutputOpenGL::OutputOpenGL() = default;

RtoR::OutputOpenGL::OutputOpenGL(const Real xMin, const Real xMax,
                                 const Real phiMin, const Real phiMax)
                                 : mFieldsGraph(xMin, xMax, phiMin, phiMax, "Fields")
{
    initialize(xMin, xMax, phiMin, phiMax);
}

void RtoR::OutputOpenGL::initialize(Real xMin, Real xMax, Real phiMin, Real phiMax) {
    if(isInitialized) return;

    //this->mTotalEnergyGraph = {0, Allocator::getInstance().getNumericParams().gett(), 0, 1,
    //                           "U, K, W, |phi|, W+|phi|", true};
    //this->mTotalEnergyGraph.addFunction(&UHistory, U_color);
    //this->mTotalEnergyGraph.addFunction(&KHistory, K_color);
    //this->mTotalEnergyGraph.addFunction(&WHistory, W_color);
    //this->mTotalEnergyGraph.addFunction(&VHistory, V_color);

    //window = new Window; window->addArtist(&this->mTotalEnergyGraph);
    //panel->addWindow(window);

    panel.addWindow(&this->stats);

    auto samples = (int)Numerics::Allocator::GetInstance().getNumericParams().getN();
    // mFieldsGraph = new GraphRtoR{xMin, xMax, -0.1*phiMax, phiMax, "AAA", true, samples};
    panel.addWindow(&mFieldsGraph, true, 0.7);

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
        const auto &p = Numerics::Allocator::GetInstance().getNumericParams();
        auto xMin = p.getxLeft();
        auto xMax = xMin + p.getL();

        initialize(xMin, xMax, -0.08, 0.08);
    }


    stats.addVolatileStat("");
    stats.addVolatileStat(std::string("t = ") + std::to_string(getLastSimTime()));
    static size_t lastStep = 0;
    stats.addVolatileStat(std::string("step ") + std::to_string(lastData.getSteps()));
    stats.addVolatileStat(std::string("delta step: ") + std::to_string(lastData.getSteps() - lastStep));
    lastStep = lastData.getSteps();



    // *************************** FIELD ***********************************
    //mFieldsGraph.draw();
    const RtoR::FieldState &fieldState = *lastData.getEqStateData<RtoR::FieldState>();

    mFieldsGraph.clearFunctions();

    if(showPhi){
        const Styles::Color colorPhi = V_color;

        mFieldsGraph.addFunction(&energyCalculator.getPotential(), "|phi|",
                                  Styles::GetColorScheme()->funcPlotStyles[0]);
    }

    if(showKineticEnergy){
        const Styles::Color colorKinetic = K_color;

        // mFieldsGraph.addFunction(&energyCalculator.getKinetic(), colorKinetic);
        mFieldsGraph.addFunction(&fieldState.getDPhiDt(), "K",
                                  Styles::GetColorScheme()->funcPlotStyles[1]);
    }

    if(showGradientEnergy) {
        const Styles::Color colorGradient = W_color;

        mFieldsGraph.addFunction(&energyCalculator.getGradient(), "grad^2",
                                  Styles::GetColorScheme()->funcPlotStyles[2]);
    }

    if(showEnergyDensity && 0){
        const Styles::Color color = U_color;
        mFieldsGraph.addFunction(&energyCalculator.getEnergy(), "E",
                                  Styles::GetColorScheme()->funcPlotStyles[3]);
    }

    panel.draw();

}

void RtoR::OutputOpenGL::_out(const OutputPacket &outInfo) {

    const RtoR::FieldState &fieldState = *outInfo.getEqStateData<RtoR::FieldState>();
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
    //    const Real k = 1-1.e-5;
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

    Graphics::OutputOpenGL::_out(outInfo);
}


bool RtoR::OutputOpenGL::notifyKeyboard(unsigned char key, int x, int y) {

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
            Log::Attention() << "Important: energy density is computing with the signum potential only." << std::endl;
            break;
        default:
            break;
    }

}








