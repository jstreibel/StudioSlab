//
// Created by joao on 7/12/22.
//

#include "ThermalMonitor.h"

#include "3rdParty/imgui/imgui.h"

#include "Mappings/RtoR/Model/FunctionsCollection/IntegerPowerFunctions.h"
#include "Mappings/RtoR/Calc/Histogram.h"

#include "Base/Controller/Interface/InterfaceManager.h"
#include "Base/Tools/Log.h"
#include "Base/Graphics/Window/WindowContainer/WindowColumn.h"

#include <sstream>
#include <array>

auto max(auto a, auto b) { return ((a)>(b)?(a):(b)); }
auto min(auto a, auto b) { return ((a)<(b)?(a):(b)); }

// Don't touch these:
#define ADD_NEW_COLUMN true
#define MANUAL_REVIEW_OF_GRAPH_LIMITS false
#define ODD_PERMUTATION true

// Ok to touch these:
#define SHOW_ENERGY_HISTORY_AS_DENSITIES true
#define HISTOGRAM_SHOULD_BE_PRETTY false
#define UPDATE_HISTORY_EVERY_STEP true


RtoR::Thermal::Monitor::Monitor(const NumericConfig &params1, KGEnergy &hamiltonian)
: RtoR::Monitor(params1, hamiltonian,  -1, 1, "thermal monitor", SHOW_ENERGY_HISTORY_AS_DENSITIES)
, mTemperaturesGraph("T")
, mHistogramsGraphK("k histogram", MANUAL_REVIEW_OF_GRAPH_LIMITS)
, mHistogramsGraphGrad("w histogram", MANUAL_REVIEW_OF_GRAPH_LIMITS)
, mHistogramsGraphV("v histogram", MANUAL_REVIEW_OF_GRAPH_LIMITS)
, mHistogramsGraphE("e histogram", MANUAL_REVIEW_OF_GRAPH_LIMITS)
, mCorrelationGraph()
{
    auto style = Styles::GetColorScheme()->funcPlotStyles.begin();
    mTemperaturesGraph.addPointSet(DummyPtr(temperature1HistoryData), (*style++).permuteColors(), "T_1");
    mTemperaturesGraph.addPointSet(DummyPtr(temperature2HistoryData), (*style++).permuteColors(), "T = 2<K>/L");
    mTemperaturesGraph.addPointSet(DummyPtr(temperature3HistoryData), (*style++).permuteColors(), "T_3");
    panel.addWindowToColumn(&mTemperaturesGraph, 0);

    style = Styles::GetColorScheme()->funcPlotStyles.begin();
    mHistogramsGraphE.addPointSet(DummyPtr(histogramEData),       *style++, "E");
    mHistogramsGraphK.addPointSet(DummyPtr(histogramKData),       *style++, "K");
    mHistogramsGraphGrad.addPointSet(DummyPtr(histogramGradData), *style++, "grad");
    mHistogramsGraphV.addPointSet(DummyPtr(histogramVData),       *style++, "V");

    auto *histogramsPanel = new WindowPanel();
    histogramsPanel->addWindow(&mHistogramsGraphV);
    histogramsPanel->addWindow(&mHistogramsGraphGrad);
    histogramsPanel->addWindow(&mHistogramsGraphK, ADD_NEW_COLUMN);
    histogramsPanel->addWindow(&mHistogramsGraphE);

    panel.addWindow(histogramsPanel);

    auto windowColumn = new WindowColumn;
    windowColumn->addWindow(DummyPtr(mFullHistoryDisplay));
    windowColumn->addWindow(DummyPtr(mCorrelationGraph), 0.2);

    panel.addWindow(windowColumn, ADD_NEW_COLUMN);

    panel.setColumnRelativeWidth(1,-1.0);
    panel.setColumnRelativeWidth(0, 0.2);
    panel.setColumnRelativeWidth(2, 0.2);
}

void RtoR::Thermal::Monitor::draw() {

    // *************************** Full history ***************************
    static bool isSetup = false;
    if( simulationHistory != nullptr ) {
        auto L = params.getL();
        auto _xMin = params.getxMin() - 0.1*L;
        auto _xMax = params.getxMax() + 0.1*L;

        static RtoR2::StraightLine straightLine;

        straightLine = RtoR2::StraightLine({_xMin, t_history}, {_xMax, t_history});

        if( not isSetup ) {
            mFullHistoryDisplay.setup(simulationHistory);

            auto style = Styles::GetColorScheme()->funcPlotStyles[2].permuteColors();
            style.lineWidth = 3;
            mFullHistoryDisplay.addCurve(DummyPtr(straightLine), style, "t_history");

            isSetup = true;
        }

        static Real stepMod, lastStepMod=0;
        stepMod = (Real)(step%(this->getnSteps()*100));
        if(stepMod < lastStepMod || UPDATE_HISTORY_EVERY_STEP) mFullHistoryDisplay.set_t(t);
        lastStepMod = stepMod;
    }


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
    auto L = params.getL();

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
    stats.addVolatileStat(Str("tau = <dotphi^2> = 2K/L = ") + ToStr(tau, 2),      (style++)->lineColor.permute());
    stats.addVolatileStat(Str("tau* = u - barphi/2 = ") + ToStr(tau_indirect, 2), (style++)->lineColor.permute());
    stats.addVolatileStat(Str("tau** = barphi + w = ") + ToStr((barϕ+2*W/L), 2),  (style++)->lineColor.permute());

    mTemperaturesGraph.set_xMax(t);

    RtoR::Monitor::draw();
}

void RtoR::Thermal::Monitor::setTransientGuess(Real guess) {
    transientGuess = guess;

    static auto line = Spaces::PointSet::New();
    line->clear();
    line->addPoint({guess, -1.0});
    line->addPoint({guess, 1.e7 });

    mEnergyGraph      .addPointSet(line, Styles::GetColorScheme()->funcPlotStyles[0].permuteColors(true).permuteColors(), "Transient", false);
    mTemperaturesGraph.addPointSet(line, Styles::GetColorScheme()->funcPlotStyles[0].permuteColors(true).permuteColors(), "Transient", false);
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
