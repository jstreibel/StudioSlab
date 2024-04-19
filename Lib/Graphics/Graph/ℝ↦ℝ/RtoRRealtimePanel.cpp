//
// Created by joao on 18/04/24.
//

#include "RtoRRealtimePanel.h"

#include <utility>

#include "3rdParty/imgui/imgui.h"
#include "Core/Tools/Log.h"
#include "Models/KleinGordon/KGSolver.h"
#include "Maps/RtoR/Calc/FourierTransform.h"
// #include "Maps/R2toR/FourierTransform.h"

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
, mFullHistoryDisplay("ϕ(t,x)")
, mFullSpaceFTHistoryDisplay("F[ϕ(t)](k)")
{
    auto currStyle = Math::StylesManager::GetCurrent();

    {
        auto sty = currStyle->funcPlotStyles.begin();

        mEnergyGraph.addPointSet(DummyPtr(UHistoryData), *sty, CHOOSE_ENERGY_LABEL("U", "U/L"));
        mEnergyGraph.addPointSet(DummyPtr(KHistoryData), *++sty, CHOOSE_ENERGY_LABEL("K", "K/L"));
        mEnergyGraph.addPointSet(DummyPtr(WHistoryData), *++sty,
                                 CHOOSE_ENERGY_LABEL("∫(𝜕ₓϕ)²dx/2", "<(𝜕ₓϕ)²>/2"));
        mEnergyGraph.addPointSet(DummyPtr(VHistoryData), *++sty,
                                 CHOOSE_ENERGY_LABEL("∫V(ϕ)dx", "<V(ϕ)>"));

        addWindow(DummyPtr(mFieldsGraph));
        addWindow(DummyPtr(mHistorySliceGraph));
        addWindow(DummyPtr(mSpaceFourierModesGraph));

        mSpaceFourierModesGraph.getAxisArtist().setHorizontalUnit(Constants::π);
    }

    {
        addWindow(DummyPtr(mEnergyGraph));
    }

    {
        auto style = currStyle->funcPlotStyles[2].permuteColors();
        style.thickness = 3;
        mFullHistoryDisplay.addCurve(DummyPtr(historyLine), style, "t_history");
        mFullHistoryDisplay.setColorMap(Styles::ColorMaps["BrBG"].inverse());

        mFullSpaceFTHistoryDisplay.setColorMap(Styles::ColorMaps["blues"].inverse().bgr());
        mFullSpaceFTHistoryDisplay.getAxisArtist().setHorizontalUnit(Constants::π);
        mFullSpaceFTHistoryDisplay.getAxisArtist().setHorizontalAxisLabel("k");
        mFullSpaceFTHistoryDisplay.getAxisArtist().setVerticalAxisLabel("t");

        if(true) {
            auto windowColumn = new ::Graphics::WindowColumn;
            windowColumn->addWindow(DummyPtr(mFullHistoryDisplay));
            windowColumn->addWindow(DummyPtr(mFullSpaceFTHistoryDisplay));
            addWindow(Window::Ptr(windowColumn), ADD_NEW_COLUMN);
        } else {
            addWindow(DummyPtr(mFullHistoryDisplay), true, 0.15);
            addWindow(DummyPtr(mFullSpaceFTHistoryDisplay), true, 0.15);
        }
    }

    setColumnRelativeWidth(0, 0.4);
    setColumnRelativeWidth(1,-1);
}

void RtoR::RealtimePanel::draw() {
    fix V_str = hamiltonian.getThePotential()->symbol();

    int errorCount = 0;

    CHECK_GL_ERRORS(errorCount++)
    updateHistoryGraphs();
    CHECK_GL_ERRORS(errorCount++)
    updateFTHistoryGraph();
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

    Graphics::WindowPanel::draw();
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

void RtoR::RealtimePanel::setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simHistory) {
    mHistorySliceGraph.setResolution(simHistory->getN());

    RtoRPanel::setSimulationHistory(simHistory);
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

    static auto section = RtoR2::StraightLine({xMin, t_history}, {xMax, t_history}, xMin, xMax);

    static bool isSetup = false;
    bool updateSamples = false;
    if( not isSetup ) {
        mHistorySectionFunc = RtoR::Section1D(simulationHistory, DummyPtr(section));
        mFullHistoryDisplay.addFunction(simulationHistory, "ϕ(t,x)");

        auto &phi = lastData.getEqStateData<RtoR::EquationState>()->getPhi();
        if(phi.getLaplacianType() == DiscreteFunction::Standard1D_PeriodicBorder)
            mFullHistoryDisplay.set_xPeriodicOn();

        isSetup = true;

        updateSamples = true;
    }

    section = RtoR2::StraightLine({xMin, t_history}, {xMax, t_history}, xMin, xMax);
    mHistorySliceGraph.clearFunctions();
    auto label = Str("ϕ(t=") + ToStr(t_history, 2) + ",x)";
    mHistorySliceGraph.addFunction(&mHistorySectionFunc, label);

    CHECK_GL_ERRORS(1)

    {
        static Real stepMod, lastStepMod = 0;
        stepMod = (Real) (lastData.getSteps() % (this->getnSteps() * 100));
        fix t = lastData.getSimTime();
        if (stepMod < lastStepMod || UPDATE_HISTORY_EVERY_STEP) mFullHistoryDisplay.set_t(t);
        lastStepMod = stepMod;
    }

    CHECK_GL_ERRORS(1.5)
}

void RtoR::RealtimePanel::updateFTHistoryGraph() {
    if(spaceFTHistory == nullptr) return;

    static bool isSetup = false;

    if( not isSetup ) {
        mFullSpaceFTHistoryDisplay.addFunction(spaceFTHistory, "ℱ[ϕ(t)](k)");

        isSetup = true;
    }

    fix step = lastData.getSteps();
    fix t = lastData.getSimTime();

    static Real stepMod, lastStepMod = 0;
    stepMod = (Real) (step % (this->getnSteps() * 100));
    if (stepMod < lastStepMod || UPDATE_HISTORY_EVERY_STEP)
        mFullSpaceFTHistoryDisplay.set_t(t);
    lastStepMod = stepMod;
}

void RtoR::RealtimePanel::updateFourierGraph() {
    static RtoR::DFTResult modes;
    using FFT = RtoR::FourierTransform;

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

        auto style = Math::StylesManager::GetCurrent()->funcPlotStyles.begin();

        mSpaceFourierModesGraph.clearPointSets();

        if(!showComplexFourier)
        {
            auto myStyle = *style;

            myStyle.thickness = 2.5;
            myStyle.primitive = Styles::VerticalLines;
            myStyle.filled = false;
            mSpaceFourierModesGraph.addPointSet(modes.getAbs(), myStyle, "|ℱ[ϕ](ω)|", false);
        }
        else
        {
            auto myStyle1 = *++style;
            auto myStyle2 = *++style;

            myStyle1.thickness = 2.5;
            myStyle1.primitive = Styles::VerticalLines;
            myStyle1.filled = false;

            myStyle2.thickness = 2.5;
            myStyle2.primitive = Styles::VerticalLines;
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
