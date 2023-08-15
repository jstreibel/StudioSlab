//
// Created by joao on 7/12/22.
//

#include "ThermalMonitor.h"

#include "Mappings/RtoR/Model/FunctionsCollection/IntegerPowerFunctions.h"
#include "Mappings/RtoR/Calc/Histogram.h"

#include "Base/Controller/Interface/InterfaceManager.h"
#include "Base/Tools/Log.h"

#include <sstream>
#include <array>

auto max(auto a, auto b) { return ((a)>(b)?(a):(b)); }
auto min(auto a, auto b) { return ((a)<(b)?(a):(b)); }

const Styles::Color T1_color = Styles::Color(0, 1, 0);
const Styles::Color T2_color = Styles::Color(1, 0.45, 1);
const Styles::Color T3_color = Styles::Color(1, 1, 0);

#define ADD_NEW_COLUMN true
#define MANUAL_REVIEW_OF_GRAPH_LIMITS false

RtoR::Thermal::Monitor::Monitor(const NumericParams &params1, KGEnergy &hamiltonian)
: RtoR::Monitor(params1, hamiltonian,  -1, 1, "thermal monitor")
, mTemperaturesGraph("T")
, mHistogramsGraphK("k histogram", MANUAL_REVIEW_OF_GRAPH_LIMITS)
, mHistogramsGraphGrad("w histogram", MANUAL_REVIEW_OF_GRAPH_LIMITS)
, mHistogramsGraphV("v histogram", MANUAL_REVIEW_OF_GRAPH_LIMITS)
, mHistogramsGraphE("e histogram", MANUAL_REVIEW_OF_GRAPH_LIMITS)
{
    panel.setColumnRelativeWidth(1, 0.60);
    panel.setColumnRelativeWidth(0, 0.10);

    auto sty = Styles::GetColorScheme()->funcPlotStyles;
    mTemperaturesGraph.addPointSet(DummyPtr(temperature1HistoryData), sty[0], "T_1");
    mTemperaturesGraph.addPointSet(DummyPtr(temperature2HistoryData), sty[1], "T = 2<K>/L");
    mTemperaturesGraph.addPointSet(DummyPtr(temperature3HistoryData), sty[2], "T_3");
    panel.addWindow(&mTemperaturesGraph, ADD_NEW_COLUMN);

    mHistogramsGraphK.addPointSet(DummyPtr(histogramKData), Styles::GetColorScheme()->funcPlotStyles[0], "K");
    mHistogramsGraphGrad.addPointSet(DummyPtr(histogramGradData), Styles::GetColorScheme()->funcPlotStyles[1], "grad");
    mHistogramsGraphV.addPointSet(DummyPtr(histogramVData), Styles::GetColorScheme()->funcPlotStyles[2], "V");
    mHistogramsGraphE.addPointSet(DummyPtr(histogramEData), Styles::GetColorScheme()->funcPlotStyles[3], "E");

    panel.addWindow(&mHistogramsGraphK);
    panel.addWindow(&mHistogramsGraphGrad);
    panel.addWindow(&mHistogramsGraphV);
    panel.addWindow(&mHistogramsGraphE);
}

void RtoR::Thermal::Monitor::draw() {

    const RtoR::EquationState &fieldState = *lastData.getEqStateData<RtoR::EquationState>();

    std::ostringstream ss;



    // *************************** Histograms *****************************
    {
        Histogram histogram;
        auto nbins = 100;

        histogram.Compute(hamiltonian.getKinetic(), nbins);
        histogramKData = histogram.asPointSet();

        histogram.Compute(hamiltonian.getGradient(), nbins);
        histogramGradData = histogram.asPointSet();

        histogram.Compute(hamiltonian.getPotential(), nbins);
        histogramVData = histogram.asPointSet();

        histogram.Compute(hamiltonian.getEnergyDensity(), nbins);
        histogramEData = histogram.asPointSet();

        if(step<5000){
            mHistogramsGraphK.reviewGraphRanges();
            mHistogramsGraphGrad.reviewGraphRanges();
            mHistogramsGraphV.reviewGraphRanges();
            mHistogramsGraphE.reviewGraphRanges();
        }
    }



    // *************************** MY BEAUTY *****************************
    auto L =       params.getL();

    auto U = hamiltonian.integrateEnergy();
    auto u = U/L;
    auto barϕ = hamiltonian.integratePotential() / L;
    auto K = hamiltonian.integrateKinetic();
    auto W = hamiltonian.integrateGradient();

    auto tau = 2*K/L;
    auto tau_indirect = u - .5*barϕ;

    stats.addVolatileStat("<\\br>");
    for(auto p : InterfaceManager::getInstance().getParametersValues({"T", "k"}) )
        stats.addVolatileStat(p.first + " = " + p.second);

    stats.addVolatileStat("<\\br>");
    ss.str(""); ss << "U = " << U;            stats.addVolatileStat(ss.str());
    ss.str(""); ss << "K = " << K;            stats.addVolatileStat(ss.str());
    ss.str(""); ss << "W = " << W;            stats.addVolatileStat(ss.str());
    ss.str(""); ss << "V = " << barϕ*L;       stats.addVolatileStat(ss.str());
    stats.addVolatileStat("");
    ss.str(""); ss << "u = U/L = " << u;        stats.addVolatileStat(ss.str());
    ss.str(""); ss << "tau = <dotphi^2> = 2K/L = " << tau;      stats.addVolatileStat(ss.str(), T1_color);
    ss.str(""); ss << "tau* = u - barphi/2 = " << tau_indirect; stats.addVolatileStat(ss.str(), T2_color);
    ss.str(""); ss << "tau** = barphi + w = " << (barϕ+2*W/L);    stats.addVolatileStat(ss.str(), T3_color);

    temperature1HistoryData.addPoint({t, tau});
    temperature2HistoryData.addPoint({t, tau_indirect});
    temperature3HistoryData.addPoint({t, barϕ + 2*W / L});

    mTemperaturesGraph.set_xMax(t);

    RtoR::Monitor::draw();

}
