//

#include "RtoRMonitor.h"

#include <utility>

#include "3rdParty/imgui/imgui.h"
#include "Core/Tools/Log.h"
#include "Models/KleinGordon/KGSolver.h"
#include "Mappings/RtoR/Model/FunctionsCollection/Section1D.h"

//
// Created by joao on 23/09/2019.


#define max(a, b) ((a)>(b)?(a):(b))
#define min(a, b) ((a)<(b)?(a):(b))

#define CHOOSE_ENERGY_LABEL(A, a) (showEnergyHistoryAsDensities ? (a) : (A))

RtoR::Monitor::Monitor(const NumericConfig &params, KGEnergy &hamiltonian,
                       const Real phiMin, const Real phiMax, Str name, bool showEnergyHistoryAsDensities)
: Core::Graphics::OpenGLMonitor(params, Str("ℝ↦ℝ ") + name)
, showEnergyHistoryAsDensities(showEnergyHistoryAsDensities)
, hamiltonian(hamiltonian)
, mFieldsGraph(params.getxMin(), params.getxMax(), phiMin, phiMax, "Fields", true, params.getN()*4)
, mEnergyGraph("Energy")
, mHistoryGraph(params.getxMin(), params.getxMax(), phiMin, phiMax, "Fields", true)
{
    auto sty = Styles::GetColorScheme()->funcPlotStyles.begin();


    mEnergyGraph.addPointSet(DummyPtr(UHistoryData), *sty,   CHOOSE_ENERGY_LABEL("U", "u"));
    mEnergyGraph.addPointSet(DummyPtr(KHistoryData), *++sty, CHOOSE_ENERGY_LABEL("K", "k"));
    mEnergyGraph.addPointSet(DummyPtr(WHistoryData), *++sty, CHOOSE_ENERGY_LABEL("Grad^2", "grad^2"));
    mEnergyGraph.addPointSet(DummyPtr(VHistoryData), *++sty, CHOOSE_ENERGY_LABEL("V", "v"));

    panel.addWindow(&mEnergyGraph);

    panel.addWindow(&mFieldsGraph, true, 0.80);
    panel.addWindow(&mHistoryGraph);
}

void RtoR::Monitor::draw() {
    // *************************** HISTORY *********************************
    if(simulationHistory != nullptr)
    {
        stats.begin();
        if(ImGui::CollapsingHeader("History")){
            if(ImGui::SliderFloat("t", &t_history, .0f, (float)getLastSimTime()))
                step_history = (int)(t_history/(float)params.gett() * (float)params.getn());

            if(ImGui::SliderInt("step", &step_history, 0, (int)lastData.getSteps()))
                t_history = (float)(step_history/(Real)params.getn() * params.gett());
        }
        stats.end();

        auto xMax = params.getxMax();
        auto xMin = params.getxMin();

        static auto section     = RtoR2::StraightLine({xMin, t_history}, {xMax, t_history}, xMin, xMax);
        static auto sectionFunc = RtoR::Section1D(simulationHistory, DummyPtr(section));

        section = RtoR2::StraightLine({xMin, t_history}, {xMax, t_history}, xMin, xMax);
        mHistoryGraph.clearFunctions();
        mHistoryGraph.addFunction(&sectionFunc, "History");
    }


    // *************************** FIELD ***********************************
    const RtoR::EquationState &fieldState = *lastData.getEqStateData<RtoR::EquationState>();

    mFieldsGraph.clearFunctions();
    hamiltonian.computeDensities(fieldState);

    if(showPhi)
        mFieldsGraph.addFunction(&hamiltonian.getPotential(), "|phi|", V_style);

    if(showKineticEnergy)
        mFieldsGraph.addFunction(&hamiltonian.getKinetic(), "K", K_style);

    if(showGradientEnergy)
        mFieldsGraph.addFunction(&hamiltonian.getGradient(), "grad^2", W_style);

    if(showEnergyDensity)
        mFieldsGraph.addFunction(&hamiltonian.getEnergyDensity(), "E", U_style);
}

void RtoR::Monitor::handleOutput(const OutputPacket &outInfo) {
    const auto t = outInfo.getSimTime();

    const RtoR::EquationState &fieldState = *outInfo.getEqStateData<RtoR::EquationState>();

    hamiltonian.computeDensities(fieldState);

    U = hamiltonian.integrateEnergy();
    K = hamiltonian.integrateKinetic();
    W = hamiltonian.integrateGradient();
    V = hamiltonian.integratePotential();

    auto factor = 1.0;
    if(showEnergyHistoryAsDensities) factor = 1./params.getL();

    UHistoryData.addPoint({t, U*factor});
    KHistoryData.addPoint({t, K*factor});
    WHistoryData.addPoint({t, W*factor});
    VHistoryData.addPoint({t, V*factor});

    auto xMax = t;

    auto yMin = min(UHistoryData.getMin().y, min(KHistoryData.getMin().y, min(WHistoryData.getMin().y, VHistoryData.getMin().y)));
    auto yMax = max(UHistoryData.getMax().y, min(KHistoryData.getMax().y, min(WHistoryData.getMax().y, VHistoryData.getMax().y)));

    mEnergyGraph.set_yMax(yMax);
    mEnergyGraph.set_yMin(yMin);

    mEnergyGraph.set_xMax(xMax);

    Core::Graphics::OpenGLMonitor::handleOutput(outInfo);
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

    return Core::Graphics::OpenGLMonitor::notifyKeyboard(key, x, y);
}

void RtoR::Monitor::setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simHistory) {
    mHistoryGraph.setResolution(simHistory->getN());
    simulationHistory = std::move(simHistory);
}








