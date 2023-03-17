//
// Created by joao on 3/16/23.
//

#include "RtoROutGLMontecarlo.h"


#include "Fields/Mappings/RtoR/Model/FunctionsCollection/IntegerPowerFunctions.h"

#include "Fields/Mappings/RtoR/Calc/Histogram.h"


#include <sstream>


#define max(a, b) ((a)>(b)?(a):(b))

const Color T1_color = Color(0, 1, 0);
const Color T2_color = Color(1, 0.45, 1);
const Color T3_color = Color(1, 1, 0);

namespace Montecarlo {

    OutGL::OutGL() {
        const Real xLeft = Allocator::getInstance().getNumericParams().getxLeft();
        const Real xRight = xLeft + Allocator::getInstance().getNumericParams().getL();

        Window *window = nullptr;

        temperature1History = RtoR::FuncArbResizable(0, 0.1);
        temperature2History = RtoR::FuncArbResizable(0, 0.1);
        temperature3History = RtoR::FuncArbResizable(0, 0.1);

        mTemperaturesGraph = new GraphRtoR(0, 2, 0, 0.01, "Temperaturas");
        mTemperaturesGraph->addFunction(&temperature1History, T1_color);
        mTemperaturesGraph->addFunction(&temperature2History, T2_color);
        mTemperaturesGraph->addFunction(&temperature3History, T3_color);
        window = new Window;
        window->addArtist(mTemperaturesGraph);
        panel->addWindow(window);


        initialize(xLeft, xRight, -0.08, 0.08);


        mHistogramsGraph = new GraphRtoR(0, 1, 0, 2e3, "V", true);
        window = new Window;
        window->addArtist(mHistogramsGraph);
        panel->addWindow(window, true, 0.2);

        mHistogramsGraph2 = new GraphRtoR(0, 1, 0, 1.2e3, "K", true);
        window = new Window;
        window->addArtist(mHistogramsGraph2);
        panel->addWindow(window);

        mHistogramsGraph3 = new GraphRtoR(0, 1, 0, 1.2e3, "(dphi/dx)^2", true);
        window = new Window;
        window->addArtist(mHistogramsGraph3);
        panel->addWindow(window);

        mHistogramsGraph4 = new GraphRtoR(0, 1, 0, 2.5e2, "U", true);
        window = new Window;
        window->addArtist(mHistogramsGraph4);
        panel->addWindow(window);


        panel->arrangeWindows();
    }

    void OutGL::draw() {
        const RtoR::FieldState &fieldState = *lastInfo.getFieldData<RtoR::FieldState>();
        if (&fieldState == nullptr) throw "Fieldstate data doesn't seem to be RtoRMap.";

        std::ostringstream ss;

        // *************************** Histograms *****************************
        Histogram histogram;

        mHistogramsGraph->clearFunctions();
        mHistogramsGraph2->clearFunctions();
        mHistogramsGraph3->clearFunctions();
        mHistogramsGraph4->clearFunctions();


        RtoR::Function *func = nullptr;

        auto nbins = 100;


        histogram.Compute(energyCalculator.getKinetic(), nbins);
        func = histogram.asPDFFunction();
        mHistogramsGraph2->addFunction(func, K_color);
        mHistogramsGraph2->xMax = histogram.xMax;
        mHistogramsGraph2->xMin = histogram.xMin;

        histogram.Compute(energyCalculator.getGradient(), nbins);
        func = histogram.asPDFFunction();
        mHistogramsGraph3->addFunction(func, W_color);
        mHistogramsGraph3->xMax = histogram.xMax;
        mHistogramsGraph3->xMin = histogram.xMin;


        histogram.Compute(energyCalculator.getPotential(), nbins);
        func = histogram.asPDFFunction();
        mHistogramsGraph->addFunction(func, V_color);
        mHistogramsGraph->xMin = histogram.xMin;
        mHistogramsGraph->xMax = histogram.xMax;

        histogram.Compute(energyCalculator.getEnergy(), nbins);
        mHistogramsGraph4->xMin = histogram.xMin;
        mHistogramsGraph4->xMax = histogram.xMax;
        func = histogram.asPDFFunction();

        mHistogramsGraph4->addFunction(func, U_color);



        // *************************** MY BEAUTY *****************************

        auto N = (Real) Allocator::getInstance().getNumericParams().getN();
        auto h = Allocator::getInstance().getNumericParams().geth();
        auto L = Allocator::getInstance().getNumericParams().getL();

        auto u = energyTotal / L;
        auto barϕ = energyCalculator.integratePotential() / L;
        auto K = energyCalculator.integrateKinetic();
        auto W = energyCalculator.integrateGradient();

        auto tau = 2 * K / L;
        auto tau_indirect = u - .5 * barϕ;


        stats.addVolatileStat(String("N = ") + std::to_string((int) N));
        stats.addVolatileStat(String("L = ") + std::to_string(L));
        stats.addVolatileStat(String("h = L/N = ") + std::to_string(L / N));
        stats.addVolatileStat(String("h = ") + std::to_string(h));
        stats.addVolatileStat(String(""));

        ss.str("");
        ss << "U = " << energyTotal;
        stats.addVolatileStat(ss.str(), U_color);
        ss.str("");
        ss << "K = " << K;
        stats.addVolatileStat(ss.str(), K_color);
        ss.str("");
        ss << "W = " << W;
        stats.addVolatileStat(ss.str(), W_color);
        ss.str("");
        ss << "V = " << barϕ * L;
        stats.addVolatileStat(ss.str(), V_color);
        stats.addVolatileStat("");
        ss.str("");
        ss << "u = U/L = " << u;
        stats.addVolatileStat(ss.str(), U_color);
        ss.str("");
        ss << "tau = <dotphi^2> = 2K/L = " << tau;
        stats.addVolatileStat(ss.str(), T1_color);
        ss.str("");
        ss << "tau* = u - barphi/2 = " << tau_indirect;
        stats.addVolatileStat(ss.str(), T2_color);
        ss.str("");
        ss << "tau** = barphi + w = " << (barϕ + 2 * W / L);
        stats.addVolatileStat(ss.str(), T3_color);

        temperature1History.insertBack(tau);
        temperature2History.insertBack(tau_indirect);
        temperature3History.insertBack(barϕ + 2 * W / L);

        auto t = lastInfo.getT();
        mTemperaturesGraph->xMax = t;
        temperature1History.xMax = mTemperaturesGraph->xMax;
        temperature2History.xMax = mTemperaturesGraph->xMax;
        temperature3History.xMax = mTemperaturesGraph->xMax;

        auto a = temperature1History.getYMax(),
                b = temperature2History.getYMax(),
                c = temperature3History.getYMax();

        mTemperaturesGraph->yMax = max(a, max(b, c));

        RtoR::OutputOpenGL::draw();

    }

}