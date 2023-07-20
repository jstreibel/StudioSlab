//
// Created by joao on 7/12/22.
//

#include "RtoROutGLStatistic.h"

#include "Mappings/RtoR/Model/FunctionsCollection/IntegerPowerFunctions.h"

#include "Mappings/RtoR/Calc/Histogram.h"

#include <sstream>

#define max(a, b) ((a)>(b)?(a):(b))

const Styles::Color T1_color = Styles::Color(0, 1, 0);
const Styles::Color T2_color = Styles::Color(1, 0.45, 1);
const Styles::Color T3_color = Styles::Color(1, 1, 0);


RtoR::OutGLStatistic::OutGLStatistic() : RtoR::Monitor(params, -1, 1) {
    const Real xLeft = params.getxLeft();
    const Real xRight = params.getxMax();

    Window *window = nullptr;

    temperature1History = FuncArbResizable(0, 0.1);
    temperature2History = FuncArbResizable(0, 0.1);
    temperature3History = FuncArbResizable(0, 0.1);

    mTemperaturesGraph = new GraphRtoR(0, 2, 0, 0.01, "Temperaturas");
    mTemperaturesGraph->addFunction(&temperature1History, "T1", Styles::GetColorScheme()->funcPlotStyles[0]);
    mTemperaturesGraph->addFunction(&temperature2History, "T2", Styles::GetColorScheme()->funcPlotStyles[1]);
    mTemperaturesGraph->addFunction(&temperature3History, "T4", Styles::GetColorScheme()->funcPlotStyles[2]);
    //window = new Window; window->addArtist(mTemperaturesGraph);
    panel.addWindow(window);

    initialize();

    mHistogramsGraph = new GraphRtoR(0, 1, 0, 2e3, "V", true);
    panel.addWindow(mHistogramsGraph, true, 0.2);

    mHistogramsGraph2 = new GraphRtoR(0, 1, 0, 1.2e3, "K", true);
    panel.addWindow(mHistogramsGraph2);

    mHistogramsGraph3 = new GraphRtoR(0, 1, 0, 1.2e3, "(dphi/dx)^2", true);
    panel.addWindow(mHistogramsGraph3);

    mHistogramsGraph4 = new GraphRtoR(0, 1, 0, 2.5e2, "U", true);
    panel.addWindow(mHistogramsGraph4);
}

void RtoR::OutGLStatistic::draw() {
    const RtoR::EquationState &fieldState = *lastData.getEqStateData<RtoR::EquationState>();

    std::ostringstream ss;

    // *************************** Histograms *****************************
    Histogram histogram;

    mHistogramsGraph->clearFunctions();
    mHistogramsGraph2->clearFunctions();
    mHistogramsGraph3->clearFunctions();
    mHistogramsGraph4->clearFunctions();


    Function* func = nullptr;

    auto nbins = 100;


    //histogram.Compute(energyCalculator.getKinetic(), nbins);
    //func = histogram.asPDFFunction();
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


    stats.addVolatileStat(Str("N = ") + std::to_string((int)N));
    stats.addVolatileStat(Str("L = ") + std::to_string(L));
    stats.addVolatileStat(Str("h = L/N = ") + std::to_string(L / N));
    stats.addVolatileStat(Str("h = ") + std::to_string(h));
    stats.addVolatileStat(Str(""));

    //ss.str(""); ss << "U = " << energyTotal;    stats.addVolatileStat(ss.str(), U_color);
    //ss.str(""); ss << "K = " << K;            stats.addVolatileStat(ss.str(), K_color);
    //ss.str(""); ss << "W = " << W;            stats.addVolatileStat(ss.str(), W_color);
    //ss.str(""); ss << "V = " << barϕ*L;      stats.addVolatileStat(ss.str(), V_color);
    stats.addVolatileStat("");
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

    auto a = temperature1History.getYMax(),
         b = temperature2History.getYMax(),
         c = temperature3History.getYMax();

    // mTemperaturesGraph->yMax = max(a, max(b,c));

    RtoR::Monitor::draw();

}
