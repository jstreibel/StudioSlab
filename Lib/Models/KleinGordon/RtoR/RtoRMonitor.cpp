//

#include "RtoRMonitor.h"

#include <utility>

#include "3rdParty/imgui/imgui.h"
#include "Core/Tools/Log.h"
#include "Models/KleinGordon/KGSolver.h"
#include "Core/Graphics/Window/WindowContainer/WindowColumn.h"
#include "Mappings/RtoR/Calc/FourierTransform.h"
// #include "Mappings/R2toR/FourierTransform.h"

//
// Created by joao on 23/09/2019.


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

// Ok to touch these:
#define SHOW_ENERGY_HISTORY_AS_DENSITIES true
#define HISTOGRAM_SHOULD_BE_PRETTY false
#define UPDATE_HISTORY_EVERY_STEP true
#define GOOD_ENOUGH_NUMBER_OF_SAMPLES 300
#define MAX_SAMPLES 50000
#define linesOffset (3 * params.getL())
#define xMinLinesInFullHistoryView (params.getxMin() - linesOffset)
#define xMaxLinesInFullHistoryView (params.getxMax() + linesOffset)

RtoR::Monitor::Monitor(const NumericConfig &params, KGEnergy &hamiltonian,
                       const Real phiMin, const Real phiMax, Str name, bool showEnergyHistoryAsDensities)
: Core::Graphics::OpenGLMonitor(params, Str("ℝ↦ℝ ") + name)
, Δt(params.gett()*0.1)
, showEnergyHistoryAsDensities(showEnergyHistoryAsDensities)
, hamiltonian(hamiltonian)
, mFieldsGraph(params.getxMin(), params.getxMax(), phiMin, phiMax, "Fields", true, (int)params.getN()*4)
, mEnergyGraph("Energy")
, mHistoryGraph(params.getxMin(), params.getxMax(), phiMin, phiMax, "Fields", true)
, mCorrelationGraph(0, params.getL(), 0, 1, "Space correlation", true, SAMPLE_COUNT(150))
{
    auto sty = Styles::GetColorScheme()->funcPlotStyles.begin();


    mEnergyGraph.addPointSet(DummyPtr(UHistoryData), *sty,   CHOOSE_ENERGY_LABEL("U", "u"));
    mEnergyGraph.addPointSet(DummyPtr(KHistoryData), *++sty, CHOOSE_ENERGY_LABEL("K", "k"));
    mEnergyGraph.addPointSet(DummyPtr(WHistoryData), *++sty, CHOOSE_ENERGY_LABEL("Grad^2", "grad^2"));
    mEnergyGraph.addPointSet(DummyPtr(VHistoryData), *++sty, CHOOSE_ENERGY_LABEL("V", "v"));

    panel.addWindow(&mEnergyGraph);

    panel.addWindow(&mFieldsGraph, true, 0.80);
    panel.addWindow(&mHistoryGraph);

    {
        auto style = Styles::GetColorScheme()->funcPlotStyles[2].permuteColors();
        style.thickness = 3;
        mFullHistoryDisplay.addCurve(DummyPtr(corrSampleLine), style, "t_history");

        auto windowColumn = new WindowColumn;
        windowColumn->addWindow(DummyPtr(mFullHistoryDisplay));
        windowColumn->addWindow(DummyPtr(mCorrelationGraph), 0.25);
        windowColumn->addWindow(DummyPtr(mSpaceFourierModesGraph), 0.25);

        panel.addWindow(windowColumn, ADD_NEW_COLUMN);
    }

    panel.setColumnRelativeWidth(1,-1.0);
    panel.setColumnRelativeWidth(0, 0.2);
    panel.setColumnRelativeWidth(2, 0.4);
}

void RtoR::Monitor::draw() {

    updateHistoryGraphs();
    updateFourierGraph();

    // ************************ RT MONITOR**********************************
    const RtoR::EquationState &fieldState = *lastData.getEqStateData<RtoR::EquationState>();

    stats.begin();
    if(ImGui::CollapsingHeader("Real-time monitor")){
        if(ImGui::Checkbox("Show V(ϕ)=|ϕ|",   &showPot))            { }
        if(ImGui::Checkbox("Show (dϕ/dt)²/2", &showKineticEnergy))  { }
        if(ImGui::Checkbox("Show (dϕ/dx)²/2", &showGradientEnergy)) { }
        if(ImGui::Checkbox("Show e",          &showEnergyDensity))  { }
    }
    stats.end();

    mFieldsGraph.clearFunctions();
    hamiltonian.computeDensities(fieldState);

    if(showPot)             mFieldsGraph.addFunction(&hamiltonian.getPotential(), "V(ϕ)=|ϕ|", V_style);
    if(showKineticEnergy)   mFieldsGraph.addFunction(&hamiltonian.getKinetic(), "K", K_style);
    if(showGradientEnergy)  mFieldsGraph.addFunction(&hamiltonian.getGradient(), "grad^2", W_style);
    if(showEnergyDensity)   mFieldsGraph.addFunction(&hamiltonian.getEnergyDensity(), "E", U_style);
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

    if(sampler == nullptr){
        fix xMin = params.getxMin();
        fix xMax = params.getxMax();

        if(false)
        {
            corrSampleLine = RtoR2::StraightLine({xMin, t_history}, {xMax, t_history});
            sampler = std::make_shared<R2toR::Sampler1D>(DummyPtr(corrSampleLine));
        } else {
            fix tl = Real2D{xMin, max(t_history-Δt, 0.0)};
            fix br = Real2D{xMax, min(t_history+Δt, params.gett())};

            sampler = std::make_shared<R2toR::RandomSampler>(tl, br, GOOD_ENOUGH_NUMBER_OF_SAMPLES);
        }

        sampler->set_nSamples(GOOD_ENOUGH_NUMBER_OF_SAMPLES);
    }

    mCorrelationFunction.setBaseFunction(simulationHistory);
    mCorrelationFunction.setSampler(sampler);

    fix L = params.getL();
    auto func2Dto1DMap = RtoR2::ParametricCurve::Ptr(new RtoR2::StraightLine({0, 0}, {L, 0}, 0, L));
    auto section = new RtoR::Section1D(DummyPtr(mCorrelationFunction), func2Dto1DMap);
    mSpaceCorrelation = RtoR::Section1D::Ptr(section);

    auto style = Styles::GetColorScheme()->funcPlotStyles[1];
    //style.filled = false; // faster (way faster in this case, because CorrelationFunction is slow to compute each value.

    // mCorrelationGraph.addFunction(mSpaceCorrelation.get(), "Space correlation", style);
    mCorrelationGraph.addPointSet(mSpaceCorrelation->renderToPointSet(), style, "Space correlation", false);
    mCorrelationGraph.setLimits({-0.05*L, L*(.5+0.05), -.5, 1});
    mCorrelationGraph.setScale(2.5e3);
}

void RtoR::Monitor::updateHistoryGraphs() {
    fix L = params.getL();
    fix xMin = params.getxMin();
    fix xMax = params.getxMax();
    fix tMax = params.gett();

    if(simulationHistory != nullptr) {
        stats.begin();
        if (ImGui::CollapsingHeader("History")) {
            if (ImGui::SliderFloat("t", &t_history, .0f, (float) getLastSimTime()))
                step_history = (int) (t_history / (float) params.gett() * (float) params.getn());

            if (ImGui::SliderInt("step", &step_history, 0, (int) lastData.getSteps()))
                t_history = (float) (step_history / (Real) params.getn() * params.gett());
        }
        stats.end();

        static auto section = RtoR2::StraightLine({xMin, t_history}, {xMax, t_history}, xMin, xMax);

        static bool isSetup = false;
        bool updateSamples = false;
        if( not isSetup ) {
            mHistorySectionFunc = RtoR::Section1D(simulationHistory, DummyPtr(section));
            mFullHistoryDisplay.setup(simulationHistory);

            isSetup = true;

            updateSamples = true;
        }

        section = RtoR2::StraightLine({xMin, t_history}, {xMax, t_history}, xMin, xMax);
        mHistoryGraph.clearFunctions();
        mHistoryGraph.addFunction(&mHistorySectionFunc, "History");

        {
            static Real stepMod, lastStepMod = 0;
            stepMod = (Real) (step % (this->getnSteps() * 100));
            if (stepMod < lastStepMod || UPDATE_HISTORY_EVERY_STEP) mFullHistoryDisplay.set_t(t);
            lastStepMod = stepMod;
        }

        {
            stats.begin();
            if (ImGui::CollapsingHeader("Correlation graph")) {
                ImGui::Text("Space correlation @ t=%f", t_history);

                auto nSamples = (int)sampler->get_nSamples();
                if(ImGui::SliderInt("samples", &nSamples, 10, MAX_SAMPLES)) {
                    sampler->set_nSamples(nSamples);
                    updateSamples = true;
                }

                auto graphResolution = (int)mCorrelationGraph.getResolution();
                if(ImGui::SliderInt("graph resolution", &graphResolution, 10, (int)simulationHistory->getN())) {
                    mCorrelationGraph.setResolution(graphResolution);
                    updateSamples = true;
                }

                fix Δt_max = params.gett();
                auto Dt = (float)Δt;
                if(ImGui::SliderFloat("sampling range (Delta t)", &Dt, (float)tMax/(float)simulationHistory->getM(), (float)Δt_max)) {
                    Δt = Dt;
                    sampler->invalidateSamples();
                    updateSamples = true;
                }
            }
            stats.end();

            static auto last_t_history = 0.0;
            if(updateSamples || (t_history != last_t_history))
            {
                {
                    corrSampleLine = RtoR2::StraightLine({xMinLinesInFullHistoryView, t_history}, {xMaxLinesInFullHistoryView, t_history});
                }

                // Sampler in correlation function
                {
                    fix tl = Real2D{xMin, max(t_history - Δt, 0.0)};
                    fix br = Real2D{xMax, min(t_history + Δt, tMax)};

                    fix nSamples = sampler->get_nSamples();
                    sampler = std::make_shared<R2toR::RandomSampler>(tl, br, nSamples);

                    mCorrelationFunction.setSampler(sampler);
                }

                {
                    mFullHistoryDisplay.clearPointSets();

                    // Correlation samples in full history
                    {
                        auto style = Styles::GetColorScheme()->funcPlotStyles[0];
                        style.primitive = Styles::Point;
                        style.thickness = 3;
                        auto ptSet = std::make_shared<Spaces::PointSet>(sampler->getSamples());
                        mFullHistoryDisplay.addPointSet(ptSet, style, "Correlation samples", MANUAL_REVIEW_GRAPH_LIMITS);
                    }

                    // Correlation graph
                    {
                        auto style = Styles::GetColorScheme()->funcPlotStyles[1];
                        mCorrelationGraph.clearPointSets();
                        auto ptSet = RtoR::FunctionRenderer::toPointSet(*mSpaceCorrelation, -.1, .5 * L * (1.1),
                                                                        mCorrelationGraph.getResolution());
                        mCorrelationGraph.addPointSet(ptSet, style, "Space correlation", MANUAL_REVIEW_GRAPH_LIMITS);
                        mCorrelationGraph.reviewGraphRanges();
                    }
                }
            }
            last_t_history = t_history;
        }
    }
}

void RtoR::Monitor::updateFourierGraph() {
    static RtoR::FTResult modes;
    using FFT = RtoR::FourierTransform;

    static auto lastStep = 0UL;

    if(step != lastStep) {
        auto &fieldState = lastData.getEqStateData<RtoR::EquationState>()->getPhi();

        modes = FFT::Compute(fieldState);

        auto style = Styles::GetColorScheme()->funcPlotStyles[1];
        style.lineColor.inverse();
        style.thickness = 5;

        mSpaceFourierModesGraph.clearFunctions();
        mSpaceFourierModesGraph.addFunction(modes.realPart.get(), "ℛℯ(ℱ[ϕ])", style);
        // mSpaceFourierModesGraph.addFunction(modes.imaginaryPart.get(), "ℐ𝓂(ℱ[ϕ])", *++style);
        mSpaceFourierModesGraph.setResolution(modes.realPart->N);

        FIRST_TIMER (
                {
                    fix xMax = modes.realPart->xMax;
                    fix xMin = modes.realPart->xMin;
                    fix Δx = xMax - xMin;
                    mSpaceFourierModesGraph.set_xMin(xMin - Δx*0.1);
                    mSpaceFourierModesGraph.set_xMax(xMax + Δx*0.1);
                })
    }

    lastStep = step;
}








