//
// Created by joao on 7/12/22.
//

#include "ThermalMonitor.h"

#include "3rdParty/imgui/imgui.h"

#include "Maps/RtoR/Model/FunctionsCollection/IntegerPowerFunctions.h"
#include "Maps/RtoR/Calc/Histogram.h"

#include "Core/Controller/Interface/InterfaceManager.h"
#include "Core/Tools/Log.h"

#include "Graphics/Graph/StylesManager.h"
#include "Graphics/Window/WindowContainer/WindowColumn.h"

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
#define xMinLinesInFullHistoryView (params.getxMin() - 3 * params.getL())
#define xMaxLinesInFullHistoryView (params.getxMax() + 3 * params.getL())


RtoR::Thermal::Monitor::Monitor(const NumericConfig &params1, KGEnergy &hamiltonian)
: ::RtoR::Monitor(params1, hamiltonian,  -1, 1, "thermal monitor", SHOW_ENERGY_HISTORY_AS_DENSITIES)
, mTemperaturesGraph("T")
, mHistogramsGraphK(   "k histogram", MANUAL_REVIEW_GRAPH_LIMITS)
, mHistogramsGraphGrad("w histogram", MANUAL_REVIEW_GRAPH_LIMITS)
, mHistogramsGraphV(   "v histogram", MANUAL_REVIEW_GRAPH_LIMITS)
, mHistogramsGraphE(   "e histogram", MANUAL_REVIEW_GRAPH_LIMITS)
{
    {
        auto style = Math::StylesManager::GetCurrent()->funcPlotStyles.begin();

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
        auto style = Math::StylesManager::GetCurrent()->funcPlotStyles.begin();
        mHistogramsGraphE.addPointSet(DummyPtr(histogramEData), *style++, "E");
        mHistogramsGraphK.addPointSet(DummyPtr(histogramKData), *style++, "K");
        mHistogramsGraphGrad.addPointSet(DummyPtr(histogramGradData), *style++, "grad");
        mHistogramsGraphV.addPointSet(DummyPtr(histogramVData), *style++, "V");


        auto *histogramsPanel = new Graphics::WindowPanel();
        histogramsPanel->addWindow(&mHistogramsGraphV);
        histogramsPanel->addWindow(&mHistogramsGraphGrad);
        histogramsPanel->addWindow(&mHistogramsGraphK, ADD_NEW_COLUMN);
        histogramsPanel->addWindow(&mHistogramsGraphE);

        panel.addWindow(histogramsPanel);
    }
}

void RtoR::Thermal::Monitor::draw() {
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

    RtoR::Monitor::draw();
}

void RtoR::Thermal::Monitor::setTransientGuess(Real guess) {
    transientGuess = guess;

    auto transientStyle = Math::StylesManager::GetCurrent()->funcPlotStyles[0].permuteColors(true).permuteColors();

    static auto vLine = Spaces::PointSet::New();
    vLine->clear();
    vLine->addPoint({guess, -1.0});
    vLine->addPoint({guess, 1.e7 });

    mEnergyGraph      .addPointSet(vLine, transientStyle, "Transient", DONT_AFFECT_RANGES);
    mTemperaturesGraph.addPointSet(vLine, transientStyle, "Transient", DONT_AFFECT_RANGES);

    // Transient in full history
    {
        auto style = Math::StylesManager::GetCurrent()->funcPlotStyles.back();
        style.filled = false;
        style.thickness = 2;

        transientLine = RtoR2::StraightLine({xMinLinesInFullHistoryView, transientGuess}, {xMaxLinesInFullHistoryView, transientGuess});
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
