//

#include "RtoRMonitor.h"

#include "Base/Tools/Log.h"
#include "Models/KleinGordon/KGSolver.h"
#include "Mappings/RtoR/Model/FunctionsCollection/AnalyticShockwave1D.h"

//
// Created by joao on 23/09/2019.


#define max(a, b) ((a)>(b)?(a):(b))
#define min(a, b) ((a)<(b)?(a):(b))


RtoR::Monitor::Monitor(const NumericParams &params, KGEnergy &hamiltonian,
                       const Real phiMin, const Real phiMax)
: Graphics::OpenGLMonitor(params, "ℝ ↦ ℝ general graphic monitor")
, hamiltonian(hamiltonian)
, mFieldsGraph(params.getxMin(), params.getxMax(), phiMin, phiMax, "Fields", true, params.getN()*4)
, mEnergyGraph("Energy")
{
    auto sty = Styles::GetColorScheme()->funcPlotStyles.begin();
    mEnergyGraph.addPointSet(DummyPtr(UHistoryData), *sty, "E");
    mEnergyGraph.addPointSet(DummyPtr(KHistoryData), *++sty, "K");
    mEnergyGraph.addPointSet(DummyPtr(WHistoryData), *++sty, "grad");
    mEnergyGraph.addPointSet(DummyPtr(VHistoryData), *++sty, "V");
    panel.addWindow(&mEnergyGraph);

    panel.addWindow(&mFieldsGraph, true, 0.80);
}

void RtoR::Monitor::draw() {
    // *************************** FIELD ***********************************
    const RtoR::EquationState &fieldState = *lastData.getEqStateData<RtoR::EquationState>();

    mFieldsGraph.clearFunctions();
    hamiltonian.computeDensities(fieldState);

    if(showPhi){
        //mFieldsGraph.addFunction(&hamiltonian.getPotential(), "|phi|", V_style);
        mFieldsGraph.addFunction(&fieldState.getPhi(), "phi", V_style);
    }

    if(showKineticEnergy){
        mFieldsGraph.addFunction(&hamiltonian.getKinetic(), "K", K_style);
        //mFieldsGraph.addFunction(&fieldState.getDPhiDt(), "K",
        //                          Styles::GetColorScheme()->funcPlotStyles[1]);
    }

    if(showGradientEnergy) {
        mFieldsGraph.addFunction(&hamiltonian.getGradient(), "grad^2", W_style);
    }

    if(showEnergyDensity){
        mFieldsGraph.addFunction(&hamiltonian.getEnergy(), "E", U_style);
    }
}

void RtoR::Monitor::_out(const OutputPacket &outInfo) {
    const auto t = outInfo.getSimTime();

    const RtoR::EquationState &fieldState = *outInfo.getEqStateData<RtoR::EquationState>();

    hamiltonian.computeDensities(fieldState);

    auto U = hamiltonian.integrateEnergy();
    auto K = hamiltonian.integrateKinetic();
    auto W = hamiltonian.integrateGradient();
    auto V = hamiltonian.integratePotential();

    //energyTotal = U;

    //if(UHistory.X.size() == 0) {
    //    // Isso eh necessario pra evitar bugs estranhos. Sorry.
    //    const Real k = 1-1.e-5;
    //    UHistory.insertBack(U * k);
    //    UHistory.insertBack(U / k);
    //}

    UHistoryData.addPoint({t, U});
    KHistoryData.addPoint({t, K});
    WHistoryData.addPoint({t, W});
    VHistoryData.addPoint({t, V});

    auto xMax = t;

    auto yMin = min(UHistoryData.getMin().y, min(KHistoryData.getMin().y, min(WHistoryData.getMin().y, VHistoryData.getMin().y)));
    auto yMax = max(UHistoryData.getMax().y, min(KHistoryData.getMax().y, min(WHistoryData.getMax().y, VHistoryData.getMax().y)));

    mEnergyGraph.set_yMax(yMax);
    mEnergyGraph.set_yMin(yMin);

    mEnergyGraph.set_xMax(xMax);

    Graphics::OpenGLMonitor::_out(outInfo);
}

bool RtoR::Monitor::notifyKeyboard(unsigned char key, int x, int y) {

    switch(key)
    {
        case '1':
            showPhi = !showPhi;
            return true;
        case '2':
            showKineticEnergy = !showKineticEnergy;
            return true;
        case '3':
            showGradientEnergy = !showGradientEnergy;
            return true;
        case '4':
            showEnergyDensity = !showEnergyDensity;
            if(showEnergyDensity) Log::Attention() << "Important: energy density is computing with the signum potential only." << Log::Flush;
            return true;
        default:
            break;
    }

    return Graphics::OpenGLMonitor::notifyKeyboard(key, x, y);
}








