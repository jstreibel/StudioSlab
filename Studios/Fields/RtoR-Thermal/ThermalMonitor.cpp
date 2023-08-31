//
// Created by joao on 7/12/22.
//

#include "ThermalMonitor.h"

#include "3rdParty/imgui/imgui.h"

#include "Mappings/RtoR/Model/FunctionsCollection/IntegerPowerFunctions.h"
#include "Mappings/RtoR/Calc/Histogram.h"

#include "Core/Controller/Interface/InterfaceManager.h"
#include "Core/Tools/Log.h"
#include "Core/Graphics/Window/WindowContainer/WindowColumn.h"

#include <sstream>
#include <array>

auto max(auto a, auto b) { return ((a)>(b)?(a):(b)); }
auto min(auto a, auto b) { return ((a)<(b)?(a):(b)); }

// Don't touch these:
#define ADD_NEW_COLUMN true
#define MANUAL_REVIEW_GRAPH_LIMITS false
#define DONT_AFFECT_RANGES false
#define AUTO_REVIEW_GRAPH_LIMITS true
#define ODD_PERMUTATION true
#define SAMPLE_COUNT(n) (n)

// Ok to touch these:
#define SHOW_ENERGY_HISTORY_AS_DENSITIES true
#define HISTOGRAM_SHOULD_BE_PRETTY false
#define UPDATE_HISTORY_EVERY_STEP true
#define GOOD_ENOUGH_NUMBER_OF_SAMPLES 300
#define MAX_SAMPLES 50000
#define _xMin (params.getxMin() - 3 * params.getL())
#define _xMax (params.getxMax() + 3 * params.getL())


RtoR::Thermal::Monitor::Monitor(const NumericConfig &params1, KGEnergy &hamiltonian)
: RtoR::Monitor(params1, hamiltonian,  -1, 1, "thermal monitor", SHOW_ENERGY_HISTORY_AS_DENSITIES)
, Δt(params1.gett()*0.1)
, mTemperaturesGraph("T")
, mHistogramsGraphK(   "k histogram", MANUAL_REVIEW_GRAPH_LIMITS)
, mHistogramsGraphGrad("w histogram", MANUAL_REVIEW_GRAPH_LIMITS)
, mHistogramsGraphV(   "v histogram", MANUAL_REVIEW_GRAPH_LIMITS)
, mHistogramsGraphE(   "e histogram", MANUAL_REVIEW_GRAPH_LIMITS)
, mCorrelationGraph(0, params.getL(), 0, 1, "Space correlation", true, SAMPLE_COUNT(150))
{
    {
        auto style = Styles::GetColorScheme()->funcPlotStyles.begin();

        mTemperaturesGraph.addPointSet(DummyPtr(temperature1HistoryData), (*style++).permuteColors(), "τₖ=2<K>/L");
        mTemperaturesGraph.addPointSet(DummyPtr(temperature2HistoryData), (*style++).permuteColors(), "τ");
        mTemperaturesGraph.addPointSet(DummyPtr(temperature3HistoryData), (*style++).permuteColors(), "τ₂");
        panel.addWindowToColumn(&mTemperaturesGraph, 0);

        auto T = std::stod(InterfaceManager::getInstance().getParametersValues({"T"})[0].second);
        auto pts = Spaces::Point2DVec({{-.1,T},{params.gett()+.1,T}});
        auto Tstyle = (*style++).permuteColors();
        Tstyle.filled = false;
        mTemperaturesGraph.addPointSet(std::make_shared<Spaces::PointSet>(pts), Tstyle, "T (nominal)");
    }

    {
        auto style = Styles::GetColorScheme()->funcPlotStyles.begin();
        mHistogramsGraphE.addPointSet(DummyPtr(histogramEData), *style++, "E");
        mHistogramsGraphK.addPointSet(DummyPtr(histogramKData), *style++, "K");
        mHistogramsGraphGrad.addPointSet(DummyPtr(histogramGradData), *style++, "grad");
        mHistogramsGraphV.addPointSet(DummyPtr(histogramVData), *style++, "V");


        auto *histogramsPanel = new WindowPanel();
        histogramsPanel->addWindow(&mHistogramsGraphV);
        histogramsPanel->addWindow(&mHistogramsGraphGrad);
        histogramsPanel->addWindow(&mHistogramsGraphK, ADD_NEW_COLUMN);
        histogramsPanel->addWindow(&mHistogramsGraphE);

        panel.addWindow(histogramsPanel);
    }

    {
        auto style = Styles::GetColorScheme()->funcPlotStyles[2].permuteColors();
        style.thickness = 3;
        mFullHistoryDisplay.addCurve(DummyPtr(corrSampleLine), style, "t_history");

        auto windowColumn = new WindowColumn;
        windowColumn->addWindow(DummyPtr(mFullHistoryDisplay));
        windowColumn->addWindow(DummyPtr(mCorrelationGraph), 0.4);

        panel.addWindow(windowColumn, ADD_NEW_COLUMN);
    }

    panel.setColumnRelativeWidth(1,-1.0);
    panel.setColumnRelativeWidth(0, 0.2);
    panel.setColumnRelativeWidth(2, 0.4);
}

void RtoR::Thermal::Monitor::draw() {
    fix L = params.getL();
    fix xMin = params.getxMin();
    fix xMax = params.getxMax();
    fix tMax = params.gett();

    // ****************** Full history + correlation graph ****************
    static bool isSetup = false;
    bool updateSamples = false;

    if( simulationHistory != nullptr ) {
        // auto _xMin = xMin - 0.1*L,
        //      _xMax = xMax + 0.1*L;

        // auto currTimeLine = RtoR2::StraightLine({_xMin, t_history}, {_xMax, t_history});

        if( not isSetup ) {
            mFullHistoryDisplay.setup(simulationHistory);

            isSetup = true;

            updateSamples = true;
        }

        static Real stepMod, lastStepMod=0;
        stepMod = (Real)(step%(this->getnSteps()*100));
        if(stepMod < lastStepMod || UPDATE_HISTORY_EVERY_STEP) mFullHistoryDisplay.set_t(t);
        lastStepMod = stepMod;
    }

    static auto last_t_history = 0.0;
    if(simulationHistory != nullptr) {
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

            fix Δt_max = params.gett() - transientGuess;
            auto Dt = (float)Δt;
            if(ImGui::SliderFloat("sampling range (Delta t)", &Dt, (float)tMax/simulationHistory->getM(), Δt_max)) {
                Δt = Dt;
                sampler->invalidateSamples();
                updateSamples = true;
            }
        }
        stats.end();

        if(updateSamples || (t_history != last_t_history))
        {
            {
                corrSampleLine = RtoR2::StraightLine({_xMin, t_history}, {_xMax, t_history});
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
    }
    last_t_history = t_history;


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


        histogram.Compute(hamiltonian.getKinetic(), nbins);
        histogramKData = histogram.asPDFPointSet(pretty);

        histogram.Compute(hamiltonian.getGradient(), nbins);
        histogramGradData = histogram.asPDFPointSet(pretty);

        histogram.Compute(hamiltonian.getPotential(), nbins);
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

    std::ostringstream ss;
    auto style = Styles::GetColorScheme()->funcPlotStyles.begin();
    stats.addVolatileStat("<\\br>");
    stats.addVolatileStat(Str("U = ") + ToStr(U), (style++)->lineColor);
    stats.addVolatileStat(Str("K = ") + ToStr(K), (style++)->lineColor);
    stats.addVolatileStat(Str("W = ") + ToStr(W), (style++)->lineColor);
    stats.addVolatileStat(Str("V = ") + ToStr(V), (style++)->lineColor);
    stats.addVolatileStat(Str("u = U/L = ") + ToStr(u, 2));

    style = Styles::GetColorScheme()->funcPlotStyles.begin();
    stats.addVolatileStat(Str("τₖ = <dotϕ^2> = 2K/L = ") + ToStr(tau, 2),      (style++)->lineColor.permute());
    stats.addVolatileStat(Str("τ = u - barφ/2 = ") + ToStr(tau_indirect, 2), (style++)->lineColor.permute());
    stats.addVolatileStat(Str("τ₂ = barphi + w = ") + ToStr((barϕ+2*W/L), 2),  (style++)->lineColor.permute());

    mTemperaturesGraph.set_xMax(t);

    RtoR::Monitor::draw();
}

void RtoR::Thermal::Monitor::setTransientGuess(Real guess) {
    transientGuess = guess;

    auto transientStyle = Styles::GetColorScheme()->funcPlotStyles[0].permuteColors(true).permuteColors();

    static auto vLine = Spaces::PointSet::New();
    vLine->clear();
    vLine->addPoint({guess, -1.0});
    vLine->addPoint({guess, 1.e7 });

    mEnergyGraph      .addPointSet(vLine, transientStyle, "Transient", DONT_AFFECT_RANGES);
    mTemperaturesGraph.addPointSet(vLine, transientStyle, "Transient", DONT_AFFECT_RANGES);

    // Transient in full history
    {
        auto style = Styles::GetColorScheme()->funcPlotStyles.back();
        style.filled = false;
        style.thickness = 2;

        transientLine = RtoR2::StraightLine({_xMin, transientGuess}, {_xMax, transientGuess});
        mFullHistoryDisplay.addCurve(DummyPtr(transientLine), style, "Transient");
    }
}

void RtoR::Thermal::Monitor::handleOutput(const OutputPacket &outInfo) {
    RtoR::Monitor::handleOutput(outInfo);

    auto L =       params.getL();

    u            = U/L;
    barϕ         = V / L;
    tau          = 2*K/L;
    tau_indirect = u - .5*barϕ;

    temperature1HistoryData.addPoint({t, tau});
    temperature2HistoryData.addPoint({t, tau_indirect});
    temperature3HistoryData.addPoint({t, barϕ + 2*W / L});
}

void RtoR::Thermal::Monitor::setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simulationHistory) {
    RtoR::Monitor::setSimulationHistory(simulationHistory);

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
