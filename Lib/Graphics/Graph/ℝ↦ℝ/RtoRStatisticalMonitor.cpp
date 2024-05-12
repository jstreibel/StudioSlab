//
// Created by joao on 16/04/24.
//

#include "RtoRStatisticalMonitor.h"

#include "imgui.h"

#include "Math/Function/Maps/RtoR/Calc/Histogram.h"
#include "Math/Function/Maps/R2toR/Model/Transform.h"

#include "Core/Controller/Interface/InterfaceManager.h"
#include "Graphics/Graph/ℝ²↦ℝ/R2toRFunctionRenderer.h"

#include <sstream>
#include <array>

// #include "Core/Controller/Interface/InterfaceManager.h"
// #include "Math/Function/Maps/RtoR/Calc/Histogram.h"

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

#define CORR_SCALE 1e0


RtoR::StatisticalMonitor::StatisticalMonitor(const NumericConfig &params, KGEnergy &hamiltonian, Graphics::GUIWindow &guiWindow)
: Graphics::RtoRPanel(params, guiWindow, hamiltonian, "ℝ↦ℝ statistics panel", "panel for statistic analysis of simulation data")
, Δt(params.gett()*0.1)
, hamiltonian(hamiltonian)
, mCorrelationSectionGraph(0, params.getL(), 0, 1, "Space correlation", true,
                    SAMPLE_COUNT(150))
, mTemperaturesGraph("T")
, mHistogramsGraphK(   "k histogram", MANUAL_REVIEW_GRAPH_LIMITS)
, mHistogramsGraphGrad("w histogram", MANUAL_REVIEW_GRAPH_LIMITS)
, mHistogramsGraphV(   "v histogram", MANUAL_REVIEW_GRAPH_LIMITS)
, mHistogramsGraphE(   "e histogram", MANUAL_REVIEW_GRAPH_LIMITS)
{
    addWindow(DummyPtr(mCorrelationSectionGraph));
    addWindow(DummyPtr(mCorrelationGraph));

    {
        auto style = Math::StylesManager::GetCurrent()->funcPlotStyles.begin();

        mTemperaturesGraph.addPointSet(DummyPtr(temperature1HistoryData), (*style++).permuteColors(), "τₖ=2<K>/L");
        mTemperaturesGraph.addPointSet(DummyPtr(temperature2HistoryData), (*style++).permuteColors(), "τ");
        mTemperaturesGraph.addPointSet(DummyPtr(temperature3HistoryData), (*style++).permuteColors(), "τ₂");
        // mTemperaturesGraph.addPointSet(DummyPtr(temperature4HistoryData), (*style++), "(τₖ+τ₂)/2");

        addWindowToColumn(DummyPtr(mTemperaturesGraph), 0);

        auto TParam = InterfaceManager::getInstance().getParametersValues({"T"});
        if(!TParam.empty())
        {
            auto T = std::stod(TParam[0].second);
            auto pts = Spaces::Point2DVec({{-.1,                T},
                                           {params.gett() + .1, T}});
            auto Tstyle = (*style++).permuteColors();
            Tstyle.filled = false;
            mTemperaturesGraph.addPointSet(std::make_shared<Spaces::PointSet>(pts), Tstyle, "T (nominal)");
        }
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

    // setColumnRelativeWidth(0, 0.125);
    setColumnRelativeWidth(0, -1);
    setColumnRelativeWidth(1, 0.40);
}

void RtoR::StatisticalMonitor::setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simHistory,
                                                    std::shared_ptr<Graphics::HistoryDisplay> simHistoryGraph) {
    RtoRPanel::setSimulationHistory(simHistory, simHistoryGraph);

    addWindow(simulationHistoryGraph, true, 0.20);

    if(sampler == nullptr){
        fix xMin = params.getxMin();
        fix xMax = params.getxMax();

        if(false)
        {
            // corrSampleLine = RtoR2::StraightLine({xMin, t_history}, {xMax, t_history});
            // sampler = std::make_shared<R2toR::Sampler1D>(DummyPtr(corrSampleLine));
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

    {
        mCorrelationComputed = Graphics::R2toRFunctionRenderer::renderToDiscrete(mCorrelationFunction, 300, 300,
                                                                                 {-.5*L, .5*L, 0, L},
                                                                                 CORR_SCALE);
        // mCorrelationComputed = Graphics::R2toRFunctionRenderer::renderToDiscrete(mCorrelationFunction, 100, 100,
        //                                                                          {0, .5 * L, 0, .5 * L},
        //                                                                          CORR_SCALE);
        mCorrelationGraph.addFunction(mCorrelationComputed, "xt correlation map");
    }

    {
        correlationLine = std::shared_ptr<RtoR2::StraightLine>(new RtoR2::StraightLine({0, 0}, {L * .5, 0}, 0, L * .5));
        mSpaceCorrelation = RtoR::Section1D::Ptr(new RtoR::Section1D(DummyPtr(mCorrelationFunction), correlationLine));

        auto style = Math::StylesManager::GetCurrent()->funcPlotStyles[1];
        //style.filled = false; // faster (way faster in this case,
        // because CorrelationFunction is slow to compute each value, and filled plots require more computation.

        mCorrelationSectionGraph.addPointSet(mSpaceCorrelation->renderToPointSet(), style, "Correlation", false);
        mCorrelationSectionGraph.setLimits({-0.05 * L, L * (.5 + 0.05), -.5, 1});
        mCorrelationSectionGraph.setScale(2.5e3);

        simulationHistoryGraph->addCurve(correlationLine,
                                         Math::StylesManager::GetCurrent()->funcPlotStyles[2].permuteColors(true),
                                         "Correlation reference");
    }
}

void RtoR::StatisticalMonitor::updateHistoryGraphs() {
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

    bool updateSamples = false;
    auto label = Str("ϕ(t=") + ToStr(t_history, 2) + ",x)";

    CHECK_GL_ERRORS(1)

    {
        guiWindow.begin();
        if (ImGui::CollapsingHeader("Correlation graph")) {
            ImGui::Text("Correlation @ samples");

            auto nSamples = (int)sampler->get_nSamples();
            if(ImGui::SliderInt("samples n", &nSamples, 10, MAX_SAMPLES)) {
                sampler->set_nSamples(nSamples);
                updateSamples = true;
            }

            auto graphResolution = (int)mCorrelationSectionGraph.getResolution();
            if(ImGui::SliderInt("graph resolution",
                                &graphResolution,
                                10,
                                (int)simulationHistory->getN()))
            {
                mCorrelationSectionGraph.setResolution(graphResolution);
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

            static auto θ = .0f;
            if(ImGui::SliderAngle("Correlation direction", &θ, 0.f, 180.f)){
                fix Δ = .5*Graphics::RtoRPanel::params.getL();
                fix c = cos((Real)θ);
                fix s = sin((Real)θ);

                correlationLine->getr() = {Δ*c, Δ*s};

                updateSamples = true;
            }

            ImGui::Spacing();
            if(ImGui::Button("Compute map")) {
                Graphics::R2toRFunctionRenderer::renderToDiscrete(mCorrelationFunction,
                                                                  mCorrelationComputed, CORR_SCALE);
                mCorrelationGraph.removeFunction(mCorrelationComputed);
                mCorrelationGraph.addFunction(mCorrelationComputed, "xt correlation map");
            }

        }
        guiWindow.end();

        CHECK_GL_ERRORS(2)

        static auto last_t_history = 0.0;
        if(updateSamples || (t_history != last_t_history))
        {
            // Sampler in correlation function
            {
                fix tl = Real2D{xMin, max(t_history - Δt, 0.0)};
                fix br = Real2D{xMax, min(t_history + Δt, tMax)};

                fix nSamples = sampler->get_nSamples();
                sampler = std::make_shared<R2toR::RandomSampler>(tl, br, nSamples);

                mCorrelationFunction.setSampler(sampler);
            }

            {
                simulationHistoryGraph->clearPointSets();

                // Correlation samples in full history
                {
                    auto style = Math::StylesManager::GetCurrent()->funcPlotStyles[0];
                    style.primitive = Styles::Point;
                    style.thickness = 3;
                    auto ptSet = std::make_shared<Spaces::PointSet>(sampler->getSamples());
                    simulationHistoryGraph->addPointSet(ptSet, style, "Correlation samples", MANUAL_REVIEW_GRAPH_LIMITS);
                }

                // Correlation graph
                {
                    auto style = Math::StylesManager::GetCurrent()->funcPlotStyles[1];
                    mCorrelationSectionGraph.clearPointSets();
                    auto ptSet = RtoR::FunctionRenderer::toPointSet(*mSpaceCorrelation, .0, .5 * L,
                                                                    mCorrelationSectionGraph.getResolution());
                    mCorrelationSectionGraph.addPointSet(ptSet, style, "Space correlation", MANUAL_REVIEW_GRAPH_LIMITS);
                    mCorrelationSectionGraph.reviewGraphRanges();
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

    drawGUI();

    CHECK_GL_ERRORS(errorCount++)

    fix L = params.getL();

    // *************************** Histograms *****************************
    {
        Histogram histogram;
        static auto nbins = 200;
        static auto pretty = HISTOGRAM_SHOULD_BE_PRETTY;

        guiWindow.begin();
        if(ImGui::CollapsingHeader("Probability density functions"))
        {
            ImGui::SliderInt("n bins", &nbins, 10, 2000);
            ImGui::Checkbox("Pretty bars", &pretty);
        }
        guiWindow.end();


        histogram.Compute(hamiltonian.getKineticDensity(), nbins);
        histogramKData = histogram.asPDFPointSet(pretty);

        histogram.Compute(hamiltonian.getGradientDensity(), nbins);
        histogramGradData = histogram.asPDFPointSet(pretty);

        histogram.Compute(hamiltonian.getPotentialDensity(), nbins);
        histogramVData = histogram.asPDFPointSet(pretty);

        histogram.Compute(hamiltonian.getEnergyDensity(), nbins);
        histogramEData = histogram.asPDFPointSet(pretty);

        fix t = lastData.getSimTime();

        // if(t<transientHint || transientHint<0 || 1)
        {
            mHistogramsGraphK.reviewGraphRanges();
            mHistogramsGraphGrad.reviewGraphRanges();
            mHistogramsGraphV.reviewGraphRanges();
            mHistogramsGraphE.reviewGraphRanges();
        }
    }

    // *************************** MY BEAUTY *****************************

    guiWindow.addVolatileStat("<\\br>");
    for(const auto& p : InterfaceManager::getInstance().getParametersValues({"T", "k", "i"}) ) {
        auto name = p.first;
        if(name == "i") name = "transient";
        guiWindow.addVolatileStat(name + " = " + p.second);
    }

    auto U = hamiltonian.getTotalEnergy();
    auto K = hamiltonian.getTotalKineticEnergy();
    auto W = hamiltonian.getTotalGradientEnergy();
    auto V = hamiltonian.getTotalPotentialEnergy();

    std::ostringstream ss;
    auto style = Math::StylesManager::GetCurrent()->funcPlotStyles.begin();
    guiWindow.addVolatileStat("<\\br>");
    guiWindow.addVolatileStat(Str("U = ") + ToStr(U), (style++)->lineColor);
    guiWindow.addVolatileStat(Str("K = ") + ToStr(K), (style++)->lineColor);
    guiWindow.addVolatileStat(Str("W = ") + ToStr(W), (style++)->lineColor);
    guiWindow.addVolatileStat(Str("V = ") + ToStr(V), (style++)->lineColor);
    guiWindow.addVolatileStat(Str("u = U/L = ") + ToStr(u, 2));

    style = Math::StylesManager::GetCurrent()->funcPlotStyles.begin();
    fix decimalPlaces = 3;
    guiWindow.addVolatileStat(Str("τₖ = <dotϕ^2> = 2K/L = ") + ToStr(tau, decimalPlaces),      (style++)->lineColor.permute());
    guiWindow.addVolatileStat(Str("τ = u - barφ/2 = ") + ToStr(tau_indirect, decimalPlaces), (style++)->lineColor.permute());
    guiWindow.addVolatileStat(Str("τ₂ = barphi + w = ") + ToStr((barϕ+2*W/L), decimalPlaces),  (style++)->lineColor.permute());
    // guiWindow.addVolatileStat(Str("(τₖ+τ₂)/2 = ") + ToStr((tau_avg), decimalPlaces),  (style++)->lineColor);

    fix t = lastData.getSimTime();
    mTemperaturesGraph.set_xMax(t);

    RtoRPanel::draw();
}

void RtoR::StatisticalMonitor::drawGUI() {
    guiWindow.begin();

    if(ImGui::CollapsingHeader("Statistical")) {
        float transient = transientHint;
        if (ImGui::SliderFloat("Transient hint", &transient, .0f, (float) params.gett())) {
            setTransientHint((Real) transient);
        }
    }

    guiWindow.end();
}

void RtoR::StatisticalMonitor::handleOutput(const OutputPacket &packet) {
    RtoRPanel::handleOutput(packet);

    auto L =       params.getL();

    auto U = hamiltonian.getTotalEnergy();
    auto K = hamiltonian.getTotalKineticEnergy();
    auto W = hamiltonian.getTotalGradientEnergy();
    auto V = hamiltonian.getTotalPotentialEnergy();

    u            = U/L;
    barϕ         = V / L;
    tau          = 2*K/L;
    tau_indirect = u - .5*barϕ;
    fix tau_2 = barϕ + 2*W / L;

    fix t = packet.getSimTime();

    temperature1HistoryData.addPoint({t, tau});
    temperature2HistoryData.addPoint({t, tau_indirect});
    temperature3HistoryData.addPoint({t, tau_2});
}

void RtoR::StatisticalMonitor::setTransientHint(Real value) {
    transientHint = value;
}





