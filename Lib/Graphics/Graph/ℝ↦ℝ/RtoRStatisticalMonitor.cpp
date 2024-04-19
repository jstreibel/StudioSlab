//
// Created by joao on 16/04/24.
//

#include "RtoRStatisticalMonitor.h"

#include "imgui.h"

#include "Core/Controller/Interface/InterfaceManager.h"
#include "Maps/RtoR/Calc/Histogram.h"
#include "Models/KleinGordon/RtoR/KG-RtoREquationState.h"




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



RtoR::StatisticalMonitor::StatisticalMonitor(const NumericConfig &params, KGEnergy &hamiltonian, int stepsBetweenDraws)
: OpenGLMonitor(params, "Statistical monitor", stepsBetweenDraws)
, Δt(params.gett()*0.1)
, hamiltonian(hamiltonian)
, mCorrelationGraph(0, params.getL(), 0, 1, "Space correlation", true,
                    SAMPLE_COUNT(150))
, mTemperaturesGraph("T")
, mHistogramsGraphK(   "k histogram", MANUAL_REVIEW_GRAPH_LIMITS)
, mHistogramsGraphGrad("w histogram", MANUAL_REVIEW_GRAPH_LIMITS)
, mHistogramsGraphV(   "v histogram", MANUAL_REVIEW_GRAPH_LIMITS)
, mHistogramsGraphE(   "e histogram", MANUAL_REVIEW_GRAPH_LIMITS)
{
    addWindow(DummyPtr(mCorrelationGraph));

    {
        auto style = Math::StylesManager::GetCurrent()->funcPlotStyles.begin();

        mTemperaturesGraph.addPointSet(DummyPtr(temperature1HistoryData), (*style++).permuteColors(), "τₖ=2<K>/L");
        mTemperaturesGraph.addPointSet(DummyPtr(temperature2HistoryData), (*style++).permuteColors(), "τ");
        mTemperaturesGraph.addPointSet(DummyPtr(temperature3HistoryData), (*style++).permuteColors(), "τ₂");

        addWindowToColumn(DummyPtr(mTemperaturesGraph), 0);

        auto T = std::stod(InterfaceManager::getInstance().getParametersValues({"T"})[0].second);
        auto pts = Spaces::Point2DVec({{-.1,T},{params.gett()+.1,T}});
        auto Tstyle = (*style++).permuteColors();
        Tstyle.filled = false;
        mTemperaturesGraph.addPointSet(std::make_shared<Spaces::PointSet>(pts), Tstyle, "T (nominal)");
    }

    {
        auto style = Math::StylesManager::GetCurrent()->funcPlotStyles.begin();
        mHistogramsGraphE.addPointSet(DummyPtr(histogramEData), *style++, "E");
        mHistogramsGraphK.addPointSet(DummyPtr(histogramKData), *style++, "K");
        mHistogramsGraphGrad.addPointSet(DummyPtr(histogramGradData), *style++, "grad");
        mHistogramsGraphV.addPointSet(DummyPtr(histogramVData), *style++, "V");


        auto *histogramsPanel = new Graphics::WindowPanel();
        histogramsPanel->addWindow(DummyPtr(mHistogramsGraphV));
        histogramsPanel->addWindow(DummyPtr(mHistogramsGraphGrad));
        histogramsPanel->addWindow(DummyPtr(mHistogramsGraphK));
        histogramsPanel->addWindow(DummyPtr(mHistogramsGraphE));

        addWindow(Window::Ptr(histogramsPanel), true);
    }

    setColumnRelativeWidth(0, 0.125);
    setColumnRelativeWidth(1, 0.20);
    setColumnRelativeWidth(2, -1);
    setColumnRelativeWidth(3, 0.25);
}

void RtoR::StatisticalMonitor::setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simHistory) {
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

    auto style = Math::StylesManager::GetCurrent()->funcPlotStyles[1];
    //style.filled = false; // faster (way faster in this case, because CorrelationFunction is slow to compute each value.

    // mCorrelationGraph.addFunction(mSpaceCorrelation.get(), "Space correlation", style);
    mCorrelationGraph.addPointSet(mSpaceCorrelation->renderToPointSet(), style, "Space correlation", false);
    mCorrelationGraph.setLimits({-0.05*L, L*(.5+0.05), -.5, 1});
    mCorrelationGraph.setScale(2.5e3);

}

void RtoR::StatisticalMonitor::updateHistoryGraphs() {
    if(simulationHistory == nullptr) return;

    CHECK_GL_ERRORS(0)

    fix L = params.getL();
    fix xMin = params.getxMin();
    fix xMax = params.getxMax();
    fix tMax = params.gett();

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
        mFullHistoryDisplay.addFunction(simulationHistory, "ϕ(t,x)");

        auto &phi = lastData.getEqStateData<RtoR::EquationState>()->getPhi();
        if(phi.getLaplacianType() == DiscreteFunction::Standard1D_PeriodicBorder)
            mFullHistoryDisplay.set_xPeriodicOn();

        isSetup = true;

        updateSamples = true;
    }

    section = RtoR2::StraightLine({xMin, t_history}, {xMax, t_history}, xMin, xMax);
    auto label = Str("ϕ(t=") + ToStr(t_history, 2) + ",x)";

    CHECK_GL_ERRORS(1)

    {
        static Real stepMod, lastStepMod = 0;
        stepMod = (Real) (step % (this->getnSteps() * 100));
        if (stepMod < lastStepMod || UPDATE_HISTORY_EVERY_STEP) mFullHistoryDisplay.set_t(t);
        lastStepMod = stepMod;
    }

    CHECK_GL_ERRORS(1.5)

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
            if(ImGui::SliderInt("graph resolution",
                                &graphResolution,
                                10,
                                (int)simulationHistory->getN()))
            {
                mCorrelationGraph.setResolution(graphResolution);
                updateSamples = true;
            }

            fix Δt_max = params.gett();
            auto Dt = (float)Δt;
            if(ImGui::SliderFloat("sampling range (Delta t)",
                                  &Dt,
                                  (float)tMax/(float)simulationHistory->getM(),
                                  (float)Δt_max)) {
                Δt = Dt;
                sampler->invalidateSamples();
                updateSamples = true;
            }
        }
        stats.end();

        CHECK_GL_ERRORS(2)

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
                    auto style = Math::StylesManager::GetCurrent()->funcPlotStyles[0];
                    style.primitive = Styles::Point;
                    style.thickness = 3;
                    auto ptSet = std::make_shared<Spaces::PointSet>(sampler->getSamples());
                    mFullHistoryDisplay.addPointSet(ptSet, style, "Correlation samples", MANUAL_REVIEW_GRAPH_LIMITS);
                }

                // Correlation graph
                {
                    auto style = Math::StylesManager::GetCurrent()->funcPlotStyles[1];
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
    CHECK_GL_ERRORS(3)
}

void RtoR::StatisticalMonitor::draw() {
    int errorCount = 0;

    CHECK_GL_ERRORS(errorCount++)

    updateHistoryGraphs();

    CHECK_GL_ERRORS(errorCount++)

    // ************************ RT MONITOR**********************************
    const RtoR::EquationState &fieldState = *lastData.getEqStateData<RtoR::EquationState>();


    fix L = params.getL();

    // *************************** Histograms *****************************
    {
        Histogram histogram;
        static auto nbins = 200;
        static auto pretty = HISTOGRAM_SHOULD_BE_PRETTY;

        stats.begin();
        if(ImGui::CollapsingHeader("Probability density functions"))
        {
            ImGui::SliderInt("n bins", &nbins, 10, 2000);
            ImGui::Checkbox("Pretty bars", &pretty);
        }
        stats.end();


        histogram.Compute(hamiltonian.getKineticDensity(), nbins);
        histogramKData = histogram.asPDFPointSet(pretty);

        histogram.Compute(hamiltonian.getGradientDensity(), nbins);
        histogramGradData = histogram.asPDFPointSet(pretty);

        histogram.Compute(hamiltonian.getPotentialDensity(), nbins);
        histogramVData = histogram.asPDFPointSet(pretty);

        histogram.Compute(hamiltonian.getEnergyDensity(), nbins);
        histogramEData = histogram.asPDFPointSet(pretty);

        if(t<transientGuess || transientGuess<0){
            mHistogramsGraphK.reviewGraphRanges();
            mHistogramsGraphGrad.reviewGraphRanges();
            mHistogramsGraphV.reviewGraphRanges();
            mHistogramsGraphE.reviewGraphRanges();
        }
    }

    // *************************** MY BEAUTY *****************************

    stats.addVolatileStat("<\\br>");
    for(const auto& p : InterfaceManager::getInstance().getParametersValues({"T", "k", "i"}) ) {
        auto name = p.first;
        if(name == "i") name = "transient";
        stats.addVolatileStat(name + " = " + p.second);
    }

    auto U = hamiltonian.getTotalEnergy();
    auto K = hamiltonian.getTotalKineticEnergy();
    auto W = hamiltonian.getTotalGradientEnergy();
    auto V = hamiltonian.getTotalPotentialEnergy();

    std::ostringstream ss;
    auto style = Math::StylesManager::GetCurrent()->funcPlotStyles.begin();
    stats.addVolatileStat("<\\br>");
    stats.addVolatileStat(Str("U = ") + ToStr(U), (style++)->lineColor);
    stats.addVolatileStat(Str("K = ") + ToStr(K), (style++)->lineColor);
    stats.addVolatileStat(Str("W = ") + ToStr(W), (style++)->lineColor);
    stats.addVolatileStat(Str("V = ") + ToStr(V), (style++)->lineColor);
    stats.addVolatileStat(Str("u = U/L = ") + ToStr(u, 2));

    style = Math::StylesManager::GetCurrent()->funcPlotStyles.begin();
    stats.addVolatileStat(Str("τₖ = <dotϕ^2> = 2K/L = ") + ToStr(tau, 2),      (style++)->lineColor.permute());
    stats.addVolatileStat(Str("τ = u - barφ/2 = ") + ToStr(tau_indirect, 2), (style++)->lineColor.permute());
    stats.addVolatileStat(Str("τ₂ = barphi + w = ") + ToStr((barϕ+2*W/L), 2),  (style++)->lineColor.permute());

    mTemperaturesGraph.set_xMax(t);



    OpenGLMonitor::draw();
}

void RtoR::StatisticalMonitor::handleOutput(const OutputPacket &outInfo) {
    OpenGLMonitor::handleOutput(outInfo);
}

