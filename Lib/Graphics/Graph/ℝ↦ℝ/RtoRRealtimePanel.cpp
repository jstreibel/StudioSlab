//
// Created by joao on 18/04/24.
//

#include "RtoRRealtimePanel.h"

#include <utility>

#include "3rdParty/imgui/imgui.h"
#include "Core/Tools/Log.h"
#include "Models/KleinGordon/KGSolver.h"
#include "Math/Function/RtoR/Calc/DiscreteFourierTransform.h"
// #include "Math/Function/R2toR/DFT.h"

// Don't touch these:
#define max(a, b) ((a)>(b)?(a):(b))
#define min(a, b) ((a)<(b)?(a):(b))
#define CHOOSE_ENERGY_LABEL(A, a) (showEnergyHistoryAsDensities ? (a) : (A))
#define ADD_NEW_COLUMN true
#define MANUAL_REVIEW_GRAPH_LIMITS false
#define DONT_AFFECT_RANGES false
#define AUTO_REVIEW_GRAPH_LIMITS true
#define ODD_PERMUTATION true
#define SAMPLE_COUNT(n) (n)
#define FIRST_TIMER(code)           \
    static bool firstTimer = true;  \
    if (firstTimer) {               \
        firstTimer = false;         \
        code                        \
    }
#define CHECK_GL_ERRORS(count) ::Graphics::OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__) + " from " + Common::getClassName(this) + " (" + ToStr((count)) + ")");

// Ok to touch these:
#define SHOW_ENERGY_HISTORY_AS_DENSITIES true
#define HISTOGRAM_SHOULD_BE_PRETTY false
#define UPDATE_HISTORY_EVERY_STEP true
#define GOOD_ENOUGH_NUMBER_OF_SAMPLES 300
#define MAX_SAMPLES 50000
#define linesOffset (3 * params.getL())
#define xMinLinesInFullHistoryView (params.getxMin() - linesOffset)
#define xMaxLinesInFullHistoryView (params.getxMax() + linesOffset)

RtoR::RealtimePanel::RealtimePanel(const NumericConfig &params, KGEnergy &hamiltonian,
                                   Graphics::GUIWindow &guiWindow,
                                   const Real phiMin, const Real phiMax,
                                   bool showEnergyHistoryAsDensities)
: Graphics::RtoRPanel(params, guiWindow, hamiltonian, "ℝ↦ℝ realtime monitor", "realtime monitoring of simulation state")
, showEnergyHistoryAsDensities(showEnergyHistoryAsDensities)
, mFieldsGraph( params.getxMin(), params.getxMax(),
                phiMin, phiMax, "Fields", true, (int)params.getN()*4)
, mEnergyGraph("Energy")
, mHistorySliceGraph(params.getxMin(), params.getxMax(),
                     phiMin, phiMax, "Fields", true)
{
    auto currStyle = Graphics::StylesManager::GetCurrent();

    {
        auto sty = currStyle->funcPlotStyles.begin();

        mEnergyGraph.addPointSet(Slab::DummyPointer(UHistoryData), *sty, CHOOSE_ENERGY_LABEL("U", "U/L"));
        mEnergyGraph.addPointSet(Slab::DummyPointer(KHistoryData), *++sty, CHOOSE_ENERGY_LABEL("K", "K/L"));
        mEnergyGraph.addPointSet(Slab::DummyPointer(WHistoryData), *++sty, CHOOSE_ENERGY_LABEL("∫(𝜕ₓϕ)²dx/2", "<(𝜕ₓϕ)²>/2"));
        mEnergyGraph.addPointSet(Slab::DummyPointer(VHistoryData), *++sty,
                                 CHOOSE_ENERGY_LABEL("∫V(ϕ)dx", "<V(ϕ)>"));

        addWindow(Slab::DummyPointer(mFieldsGraph));
        addWindow(Slab::DummyPointer(mHistorySliceGraph));
        addWindow(Slab::DummyPointer(mSpaceFourierModesGraph));

        mSpaceFourierModesGraph.getAxisArtist().setHorizontalUnit(Constants::π);
    }

    addWindow(Slab::DummyPointer(mEnergyGraph));

    setColumnRelativeWidth(0, 0.4);
    setColumnRelativeWidth(1, -1);
}

void RtoR::RealtimePanel::draw() {
    fix V_str = hamiltonian.getThePotential()->symbol();

    int errorCount = 0;

    CHECK_GL_ERRORS(errorCount++)
    updateHistoryGraphs();
    CHECK_GL_ERRORS(errorCount++)
    updateFourierGraph();
    CHECK_GL_ERRORS(errorCount++)

    // ************************ RT MONITOR**********************************
    guiWindow.begin();
    if(ImGui::CollapsingHeader("Real-time monitor")){

        if(ImGui::Checkbox(("Show V(ϕ)="+V_str).c_str(), &showPot))              { }
        if(ImGui::Checkbox("Show (𝜕ₜϕ)²/2", &showKineticEnergy))    { }
        if(ImGui::Checkbox("Show (𝜕ₓϕ)²/2", &showGradientEnergy))   { }
        if(ImGui::Checkbox("Show e",        &showEnergyDensity))    { }
    }
    guiWindow.end();

    mFieldsGraph.clearFunctions();

    if(showPot)             mFieldsGraph.addFunction(&hamiltonian.getPotentialDensity(), "V(ϕ)="+V_str, V_style);
    if(showKineticEnergy)   mFieldsGraph.addFunction(&hamiltonian.getKineticDensity(),   "K/L"        , K_style);
    if(showGradientEnergy)  mFieldsGraph.addFunction(&hamiltonian.getGradientDensity(),  "(𝜕ₓϕ)²"     , W_style);
    if(showEnergyDensity)   mFieldsGraph.addFunction(&hamiltonian.getEnergyDensity(),    "E/L"        , U_style);

    CHECK_GL_ERRORS(errorCount++)

    Graphics::RtoRPanel::draw();
}

void RtoR::RealtimePanel::handleOutput(const OutputPacket &outInfo) {
    const auto t = outInfo.getSimTime();

    const RtoR::EquationState &fieldState = *outInfo.getEqStateData<RtoR::EquationState>();

    //hamiltonian.computeEnergies(fieldState);

    auto U = hamiltonian.getTotalEnergy();
    auto K = hamiltonian.getTotalKineticEnergy();
    auto W = hamiltonian.getTotalGradientEnergy();
    auto V = hamiltonian.getTotalPotentialEnergy();

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

}

bool RtoR::RealtimePanel::notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) {
    if(state == Core::Press) switch(key)
        {
            case '1':
                showPot = !showPot;
                return true;
            case '2':
                showKineticEnergy = !showKineticEnergy;
                return true;
            case '3':
                showGradientEnergy = !showGradientEnergy;
                return true;
            case '4':
                showEnergyDensity = !showEnergyDensity;
                return true;
            default:
                break;
        }

    return WindowPanel::notifyKeyboard(key, state, modKeys);
}

void RtoR::RealtimePanel::setSimulationHistory(R2toR::DiscreteFunction_constptr simHistory,
                                               Graphics::HistoryDisplay_ptr simHistoryGraph) {
    mHistorySliceGraph.setResolution(simHistory->getN());

    RtoRPanel::setSimulationHistory(simHistory, simHistoryGraph);

    if(0) {
        auto style = Graphics::StylesManager::GetCurrent()->funcPlotStyles[2].permuteColors();
        style.thickness = 3;
        simulationHistoryGraph->addCurve(DummyPtr(historyLine), style, "t_history");
    }

    addWindow(simulationHistoryGraph, ADD_NEW_COLUMN);
    setColumnRelativeWidth(1,-1);
}

void RtoR::RealtimePanel::setSpaceFourierHistory(R2toR::DiscreteFunction_constptr sftHistory,
                                                 const DFTDataHistory &dftData,
                                                 Graphics::HistoryDisplay_ptr sftHistoryGraph)
{
    RtoRPanel::setSpaceFourierHistory(sftHistory, dftData, sftHistoryGraph);

    addWindow(sftHistoryGraph);
}

void RtoR::RealtimePanel::updateHistoryGraphs() {
    if(simulationHistory == nullptr) return;

    CHECK_GL_ERRORS(0)

    fix L = params.getL();
    fix xMin = params.getxMin();
    fix xMax = params.getxMax();
    fix tMax = params.gett();

    guiWindow.begin();
    if (ImGui::CollapsingHeader("History")) {
        if (ImGui::SliderFloat("t", &t_history, .0f, (float) getLastSimTime()))
            step_history = (int) (t_history / (float) params.gett() * (float) params.getn());

        if (ImGui::SliderInt("step", &step_history, 0, (int) lastData.getSteps()))
            t_history = (float) (step_history / (Real) params.getn() * params.gett());
    }
    guiWindow.end();

    CHECK_GL_ERRORS(1)
}

void RtoR::RealtimePanel::updateFourierGraph() {
    static RtoR::DFTResult modes;
    using FFT = RtoR::DFT;

    static auto lastStep = 0UL;
    fix step = lastData.getSteps();

    bool forceUpdate = false;

    guiWindow.begin();
    if (ImGui::CollapsingHeader("Fourier"))
        forceUpdate = ImGui::Checkbox("Show Fourier as complex amplitudes", &showComplexFourier);
    guiWindow.end();

    if(step != lastStep || forceUpdate) {
        auto &fieldState = lastData.getEqStateData<RtoR::EquationState>()->getPhi();

        modes = FFT::Compute(fieldState);

        auto style = Graphics::StylesManager::GetCurrent()->funcPlotStyles.begin();

        mSpaceFourierModesGraph.clearPointSets();

        if(!showComplexFourier)
        {
            auto myStyle = *style;

            myStyle.thickness = 2.5;
            myStyle.primitive = Graphics::VerticalLines;
            myStyle.filled = false;
            mSpaceFourierModesGraph.addPointSet(modes.getMagnitudes(), myStyle, "|ℱ[ϕ](ω)|", false);
        }
        else
        {
            auto myStyle1 = *++style;
            auto myStyle2 = *++style;

            myStyle1.thickness = 2.5;
            myStyle1.primitive = Graphics::VerticalLines;
            myStyle1.filled = false;

            myStyle2.thickness = 2.5;
            myStyle2.primitive = Graphics::VerticalLines;
            myStyle2.filled = false;

            mSpaceFourierModesGraph.addPointSet(modes.re, *style++, "ℜ(ℱ[ϕ](ω))", false);
            mSpaceFourierModesGraph.addPointSet(modes.im, *style, "ℑ(ℱ[ϕ](ω))", false);
        }

        FIRST_TIMER (
                {
                    fix xMax = modes.re->getMax().x;
                    fix xMin = modes.re->getMin().x;
                    fix Δx = xMax - xMin;
                    mSpaceFourierModesGraph.set_xMin(xMin - Δx*0.1);
                    mSpaceFourierModesGraph.set_xMax(xMax + Δx*0.1);
                })
    }

    lastStep = step;
}


