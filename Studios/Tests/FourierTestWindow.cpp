//
// Created by joao on 4/09/23.
//

#include "FourierTestWindow.h"

#include "Core/Tools/Log.h"

#include "3rdParty/imgui/imgui.h"

#include "Math/Constants.h"
#include "Math/Function/NativeFunction.h"

#include "Maps/RtoR/Model/RtoRFunction.h"
#include "Maps/RtoR/Model/RtoRDiscreteFunctionCPU.h"

#include "Maps/RtoR/Calc/FourierTransform.h"
#include "Maps/RtoR/Model/FunctionsCollection/InverseFourier.h"
#include "Maps/FunctionRenderer.h"
#include "Maps/RtoR/Model/FunctionsCollection/ComplexMagnitude.h"
#include "Maps/RtoC/FourierModes.h"

#define Re 0
#define Im 1

namespace Tests {
    fix π = (float)Constants::pi;

    float A = 1;
    float ω = 1;
    float ωₚₑₐₖ[2] = {0.0, ω};
    int N_modes = 1000;
    float L = 2*π;
    fix ωₘₐₓ = 6*π;
    fix xMin = - .1*L;
    fix xMax =  1.1*L;

    float ω₀ = 0.0;
    int decimation = 1;

    int N_sqrWave = 20;


    const char* funcSymbol = "sin";

    Real func(Real x) {
        if(0) {
            Real val = 0.0;
            for (int i = 0; i < N_sqrWave; ++i) {
                int n = 2 * i + 1;
                val += sin(n * ω * x) / (Real) n;
            }
            return 2 * val / π;
        } else return A*sin(ω*x);
    }

    Core::NativeFunction<RtoR::Function> Func(func);
    RtoR::InverseFourier FuncRebuilt;
    RtoC::FourierModes fourierModes(DummyPtr(Func), 0, L, 100);
    RtoR::ComplexMagnitude amplitudes(DummyPtr(fourierModes));

    FourierTestWindow::FourierTestWindow()
    : mFuncGraph(xMin, xMax, -3.25*A, 3.25*A, "func graph", false, N_modes)
    , mFTGraph(-1.1*ω, 1.1*ω, -0.1, 1.1, "", false, 200)
    {
        updateGraphs();

        mFTGraph.addFunction(&amplitudes, Str("ℱ[") + funcSymbol + "](k)", Styles::GetCurrent()->funcPlotStyles[4]);

        row.addWindow(DummyPtr(gui), WindowRow::Right, .25);

        col.addWindow(DummyPtr(mFTGraph));
        col.addWindow(DummyPtr(mDFTGraph));
        col.addWindow(DummyPtr(mFuncGraph));
        row.addWindow(DummyPtr(col));
    }

    void FourierTestWindow::draw() {
        gui.begin();
        if( ImGui::SliderFloat("ω", &ω, 0.1, ωₘₐₓ)
          | ImGui::SliderFloat("L", &L, 0.1, ωₘₐₓ)
          | ImGui::DragInt("N", &N_modes, (float)N_modes / 20.f, 1, 20000)
          | ImGui::DragInt("Nₛ", &N_sqrWave, 1, 1, 100)
          | ImGui::SliderFloat("ω₀", &ω₀, 0, ωₘₐₓ)
          | ImGui::SliderInt("dec", &decimation, 1, N_modes))
            updateGraphs();


        gui.end();

        gui.addVolatileStat(Str("L = ")    + ToStr(L /π, 2) + "π = " + ToStr(L) );
        gui.addVolatileStat(Str("ω = ")    + ToStr(ω /π, 2) + "π = " + ToStr(ω, 2));
        gui.addVolatileStat(Str("Re(ωₚₑₐₖ) = ")   + ToStr(ωₚₑₐₖ[Re]/π, 2) + "π = " + ToStr(ωₚₑₐₖ[Re], 2));
        gui.addVolatileStat(Str("Im(ωₚₑₐₖ) = ")    + ToStr(ωₚₑₐₖ[Im]/π, 2) + "π = " + ToStr(ωₚₑₐₖ[Im], 2));
        gui.addVolatileStat(Str("Re(ωₚₑₐₖ)/ω = ") + ToStr(ωₚₑₐₖ[Re]/ω, 2) );
        gui.addVolatileStat(Str("Im(ωₚₑₐₖ)/ω = ") + ToStr(ωₚₑₐₖ[Im]/ω, 2) );

        row.draw();
    }

    void FourierTestWindow::notifyReshape(int w, int h) {
        Window::notifyReshape(w,h);

        row.setx(getx());
        row.sety(gety());

        row.notifyReshape(w, h);
    }

    void FourierTestWindow::updateGraphs() {
        static RtoR::DFTResult modes;
        using FFT = RtoR::FourierTransform;

        fourierModes.setL(L);
        fourierModes.setNSamples(N_modes);

        {

            modes = FFT::Compute(Func, N_modes, 0, L);

            ωₚₑₐₖ[Re] = 0.0;
            Real A_max = 0.0;
            for (const auto &pt: modes.re->getPoints()) {
                fix Aₖ = abs(pt.y);
                if (A_max < Aₖ) {
                    A_max = Aₖ;
                    ωₚₑₐₖ[Re] = pt.x;
                }
            }
            ωₚₑₐₖ[Im] = 0.0;
            for (const auto &pt: modes.im->getPoints()) {
                fix Aₖ = abs(pt.y);
                if (A_max < Aₖ) {
                    A_max = Aₖ;
                    ωₚₑₐₖ[Im] = pt.x;
                }
            }

            auto style = Styles::GetCurrent()->funcPlotStyles[1];
            style.lineColor.inverse();
            style.thickness = 2.5;
            style.primitive = Styles::VerticalLines;
            style.filled = false;
            style.lineColor.a = 0.8;
            mDFTGraph.clearPointSets();
            mDFTGraph.addPointSet(modes.re, style, Str("ℑ(ℱ[") + funcSymbol + "(x)])", false);

            style = Styles::GetCurrent()->funcPlotStyles[3];
            style.lineColor.inverse();
            style.thickness = 2.5;
            style.primitive = Styles::VerticalLines;
            style.filled = false;
            style.lineColor.a = 0.8;
            mDFTGraph.addPointSet(modes.im, style, Str("ℜ(ℱ[") + funcSymbol + "(x)])", false);


            if (true) {
                mDFTGraph.set_xMin(-2 * π);
                mDFTGraph.set_xMax(2*ωₘₐₓ);
            } else {
                auto min = modes.re->getMin();
                auto max = modes.re->getMax();
                auto Δx = max.x - min.x;

                mDFTGraph.set_xMin(min.x - Δx * 0.15);
                mDFTGraph.set_xMax(max.x + Δx * 0.15);
            }

            if (true) {
                mDFTGraph.set_yMax(1.25);
                mDFTGraph.set_yMin(-1.25);
            } else {
                auto min_Re = modes.re->getMin();
                auto max_Re = modes.re->getMax();
                auto Δy_Re = max_Re.y - min_Re.y;
                fix range_Re = std::max(abs(max_Re.y), abs(min_Re.y));

                auto min_Im = modes.im->getMin();
                auto max_Im = modes.im->getMax();
                auto Δy_Im = max_Im.y - min_Im.y;
                fix range_Im = std::max(abs(max_Im.y), abs(min_Im.y));

                fix range = std::max(range_Re, range_Im);
                fix Δy = std::max(Δy_Re, Δy_Im);

                mDFTGraph.set_yMin(-range - Δy * 0.15);
                mDFTGraph.set_yMax(+range + Δy * 0.15);
            }
        }


        {
            mFuncGraph.clearFunctions();

            auto style = Styles::GetCurrent()->funcPlotStyles[0].permuteColors(true);
            style.filled = false;
            style.thickness = 5;
            mFuncGraph.addFunction(&Func, funcSymbol, style);

            style = Styles::GetCurrent()->funcPlotStyles[1].permuteColors(true);
            style.filled = false;
            style.thickness = 5;
            FuncRebuilt.setModes(modes);
            mFuncGraph.addFunction(&FuncRebuilt, Str("ℱ⁻¹[ℱ[")+funcSymbol+"(x)]]", style);

            mFuncGraph.setResolution(N_modes);
            mFuncGraph.set_xMin(-0.1 * L);
            mFuncGraph.set_xMax(1.1 * L);

            static Spaces::PointSet L_loc1, L_loc2;
            static bool initd = false;
            if (!initd) {
                L_loc1.setPoints({{0.0, -2},
                                  {0.0, +2}});

                mFuncGraph.addPointSet(DummyPtr(L_loc1), Styles::GetCurrent()->funcPlotStyles[3], "xₘᵢₙ", false);
                mFuncGraph.addPointSet(DummyPtr(L_loc2), Styles::GetCurrent()->funcPlotStyles[3], "xₘₐₓ", false);

                initd = true;
            }

            L_loc2.setPoints({{L, -2},
                              {L, +2}});
        }
    }

    bool FourierTestWindow::notifyMouseMotion(int x, int y) {
        return row.notifyMouseMotion(x,y);
    }

    bool FourierTestWindow::notifyMouseWheel(double dx, double dy) {
        return row.notifyMouseWheel(dx, dy);
    }


} // Tests
