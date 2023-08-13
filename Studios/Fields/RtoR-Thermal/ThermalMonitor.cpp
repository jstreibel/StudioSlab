//
// Created by joao on 7/12/22.
//

#include "ThermalMonitor.h"

#include "Mappings/RtoR/Model/FunctionsCollection/IntegerPowerFunctions.h"

#include "Mappings/RtoR/Calc/Histogram.h"

#include <sstream>

#define max(a, b) ((a)>(b)?(a):(b))

const Styles::Color T1_color = Styles::Color(0, 1, 0);
const Styles::Color T2_color = Styles::Color(1, 0.45, 1);
const Styles::Color T3_color = Styles::Color(1, 1, 0);


RtoR::Thermal::Monitor::Monitor(const NumericParams &params1, KGEnergy &hamiltonian)
: RtoR::Monitor(params1, hamiltonian,  -1, 1)
, mTemperaturesGraph("T")
, mHistogramsGraph1("u")
, mHistogramsGraph2("k")
, mHistogramsGraph3("w")
, mHistogramsGraph4("v")
{
    panel.setColumnRelativeWidth(1, 0.60);
    panel.setColumnRelativeWidth(0, 0.10);

    auto sty = Styles::GetColorScheme()->funcPlotStyles;
    mTemperaturesGraph.addPointSet(DummyPtr(temperature1HistoryData), sty[0], "T_1");
    mTemperaturesGraph.addPointSet(DummyPtr(temperature2HistoryData), sty[1], "T_2");
    mTemperaturesGraph.addPointSet(DummyPtr(temperature3HistoryData), sty[2], "T_3");
    panel.addWindow(&mTemperaturesGraph);

    panel.addWindow(&mHistogramsGraph1);
    panel.addWindow(&mHistogramsGraph2);
    panel.addWindow(&mHistogramsGraph3);
    panel.addWindow(&mHistogramsGraph4);
}

void RtoR::Thermal::Monitor::draw() {

    const RtoR::EquationState &fieldState = *lastData.getEqStateData<RtoR::EquationState>();

    std::ostringstream ss;

    // *************************** Histograms *****************************
    Histogram histogram;

    auto nbins = 100;

    histogram.Compute(hamiltonian.getKinetic(), nbins);
    temperature1HistoryData = histogram.asPointSet();
    //mHistogramsGraph2->addFunction(func, "K histogram", Styles::GetColorScheme()->funcPlotStyles[0]);
    //mHistogramsGraph2->xMax = histogram.xMax;
    //mHistogramsGraph2->xMin = histogram.xMin;

    //histogram.Compute(energyCalculator.getGradient(), nbins);
    //func = histogram.asPDFFunction();
    //mHistogramsGraph3->addFunction(func, "grad histogram", Styles::GetColorScheme()->funcPlotStyles[0]);
    //mHistogramsGraph3->xMax = histogram.xMax;
    //mHistogramsGraph3->xMin = histogram.xMin;


    //histogram.Compute(energyCalculator.getPotential(), nbins);
    //func = histogram.asPDFFunction();
    //mHistogramsGraph->addFunction(func, "V histogram", Styles::GetColorScheme()->funcPlotStyles[0]);
    //mHistogramsGraph->xMin = histogram.xMin;
    //mHistogramsGraph->xMax = histogram.xMax;

    //histogram.Compute(energyCalculator.getEnergy(), nbins);
    //mHistogramsGraph4->xMin = histogram.xMin;
    //mHistogramsGraph4->xMax = histogram.xMax;
    //func = histogram.asPDFFunction();
    //mHistogramsGraph4->addFunction(func, "E histogram", Styles::GetColorScheme()->funcPlotStyles[0]);



    // *************************** MY BEAUTY *****************************

    auto N = (Real)params.getN();
    auto h =       params.geth();
    auto L =       params.getL();

    //auto u = energyTotal/L;
    //auto barϕ = energyCalculator.integratePotential() / L;
    //auto K = energyCalculator.integrateKinetic();
    //auto W = energyCalculator.integrateGradient();

    //auto tau = 2*K/L;
    //auto tau_indirect = u - .5*barϕ;

    //ss.str(""); ss << "U = " << energyTotal;    stats.addVolatileStat(ss.str(), U_color);
    //ss.str(""); ss << "K = " << K;            stats.addVolatileStat(ss.str(), K_color);
    //ss.str(""); ss << "W = " << W;            stats.addVolatileStat(ss.str(), W_color);
    //ss.str(""); ss << "V = " << barϕ*L;      stats.addVolatileStat(ss.str(), V_color);
    //stats.addVolatileStat("");
    //ss.str(""); ss << "u = U/L = " << u;        stats.addVolatileStat(ss.str(), U_color);
    //ss.str(""); ss << "tau = <dotphi^2> = 2K/L = " << tau;      stats.addVolatileStat(ss.str(), T1_color);
    //ss.str(""); ss << "tau* = u - barphi/2 = " << tau_indirect; stats.addVolatileStat(ss.str(), T2_color);
    //ss.str(""); ss << "tau** = barphi + w = " << (barϕ+2*W/L);    stats.addVolatileStat(ss.str(), T3_color);

    //temperature1History.insertBack(tau);
    //temperature2History.insertBack(tau_indirect);
    //temperature3History.insertBack(barϕ + 2*W / L);

    auto t = lastData.getSimTime();
    //mTemperaturesGraph->xMax = t;
    //temperature1History.xMax = mTemperaturesGraph->xMax;
    //temperature2History.xMax = mTemperaturesGraph->xMax;
    //temperature3History.xMax = mTemperaturesGraph->xMax;

    // auto a = temperature1History.getYMax(),
    //      b = temperature2History.getYMax(),
    //      c = temperature3History.getYMax();

    // mTemperaturesGraph->yMax = max(a, max(b,c));

    RtoR::Monitor::draw();

}
