//
// Created by joao on 4/09/23.
//

#include "FourierTestWindow.h"


#include "3rdParty/imgui/imgui.h"

#include "Math/Constants.h"
#include "Math/Function/NativeFunction.h"
#include "Maps/RtoR/Model/RtoRFunction.h"
#include "Maps/FunctionRenderer.h"
#include "Core/Tools/Log.h"
#include "Maps/RtoR/Calc/FourierTransform.h"

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

    Real func(Real x) { return A* sin(ω*x); }

    Core::NativeFunction<Real, Real> Func(func);

    FourierTestWindow::FourierTestWindow() : mFuncGraph(xMin, xMax, -1.25*A, 1.25*A, "func graph", false, N_modes) {
        auto &panel = row;

        updateGraphs();

        panel.addWindow(DummyPtr(gui), 0.2);
        panel.addWindow(DummyPtr(mFuncGraph));
        panel.addWindow(DummyPtr(mDFTGraph));
    }

    void FourierTestWindow::draw() {
        gui.begin();
        if( ImGui::SliderFloat("ω", &ω, 0.1, ωₘₐₓ)
          | ImGui::SliderFloat("L", &L, 0.1, ωₘₐₓ)
          | ImGui::DragInt("N", &N_modes, (float)N_modes / 20.f, 1, 20000))
            updateGraphs();
        gui.end();

        gui.addVolatileStat(Str("L = ")    + ToStr(L /π, 2) + "π = " + ToStr(L) );
        gui.addVolatileStat(Str("ω = ")    + ToStr(ω /π, 2) + "π = " + ToStr(ω, 2));
        gui.addVolatileStat(Str("Re(ωₚₑₐₖ) = ")   + ToStr(ωₚₑₐₖ[Re]/π, 2) + "π = " + ToStr(ωₚₑₐₖ[Re], 2));
        gui.addVolatileStat(Str("Im(ωₚₑₐₖ) = ")    + ToStr(ωₚₑₐₖ[Im]/π, 2) + "π = " + ToStr(ωₚₑₐₖ[Im], 2));
        gui.addVolatileStat(Str("Re(ωₚₑₐₖ)/ω = ") + ToStr(ωₚₑₐₖ[Re]/ω, 2) );
        gui.addVolatileStat(Str("Im(ωₚₑₐₖ)/ω = ") + ToStr(ωₚₑₐₖ[Im]/ω, 2) );

        row.draw();
        col.draw();


    }

    void FourierTestWindow::notifyReshape(int w, int h) {
        Window::notifyReshape(w,h);

        row.setx(x);
        row.sety(y);
        col.setx(x);
        col.sety(y);

        row.notifyReshape(w, h);
        col.notifyReshape(w, h);
    }

    void FourierTestWindow::updateGraphs() {
        {
            auto style = Styles::GetColorScheme()->funcPlotStyles[0].permuteColors(true);
            style.filled = false;
            style.thickness = 5;

            mFuncGraph.clearFunctions();
            mFuncGraph.addFunction(&Func, "sin(x)", style);
            mFuncGraph.setResolution(N_modes);
            mFuncGraph.set_xMin(-0.1 * L);
            mFuncGraph.set_xMax(1.1 * L);

            static Spaces::PointSet L_loc1, L_loc2;
            static bool initd = false;
            if (!initd) {
                L_loc1.setPoints({{0.0, -2},
                                  {0.0, +2}});

                mFuncGraph.addPointSet(DummyPtr(L_loc1), Styles::GetColorScheme()->funcPlotStyles[3], "xₘᵢₙ", false);
                mFuncGraph.addPointSet(DummyPtr(L_loc2), Styles::GetColorScheme()->funcPlotStyles[3], "xₘₐₓ", false);

                initd = true;
            }

            L_loc2.setPoints({{L, -2},
                              {L, +2}});
        }



        using FFT = RtoR::FourierTransform;

        static RtoR::DFTResult modes;
        modes = FFT::Compute(Func, N_modes, 0, L);

        ωₚₑₐₖ[Re] = 0.0;
        Real A_max = 0.0;
        for(const auto &pt : modes.re->getPoints()) {
            fix Aₖ = abs(pt.y);
            if (A_max < Aₖ) {
                A_max = Aₖ;
                ωₚₑₐₖ[Re] = pt.x;
            }
        }
        ωₚₑₐₖ[Im] = 0.0;
        for(const auto &pt : modes.im->getPoints()) {
            fix Aₖ = abs(pt.y);
            if (A_max < Aₖ) {
                A_max = Aₖ;
                ωₚₑₐₖ[Im] = pt.x;
            }
        }

        auto style = Styles::GetColorScheme()->funcPlotStyles[1];
        style.lineColor.inverse();
        style.thickness = 2.5;
        style.primitive = Styles::VerticalLines;
        style.filled = false;
        style.lineColor.a = 0.8;
        mDFTGraph.clearPointSets();
        mDFTGraph.addPointSet(modes.re, style, "ℑ(ℱ[sin(x)])", false);

        style = Styles::GetColorScheme()->funcPlotStyles[3];
        style.lineColor.inverse();
        style.thickness = 2.5;
        style.primitive = Styles::VerticalLines;
        style.filled = false;
        style.lineColor.a = 0.8;
        mDFTGraph.addPointSet(modes.im, style, "ℜ(ℱ[sin(x)])", false);


        if(true) {
            mDFTGraph.set_xMin(-2*π);
            mDFTGraph.set_xMax( 15*π );

        }
        else {
            auto min = modes.re->getMin();
            auto max = modes.re->getMax();
            auto Δx = max.x - min.x;

            mDFTGraph.set_xMin(min.x - Δx * 0.15);
            mDFTGraph.set_xMax(max.x + Δx * 0.15);
        }

        if(true) {
            mDFTGraph.set_yMax(1.25);
            mDFTGraph.set_yMin(-1.25);
        } else {
            auto min_Re = modes.re->getMin();
            auto max_Re = modes.re->getMax();
            auto Δy_Re = max_Re.y - min_Re.y;
            fix range_Re = std::max(abs(max_Re.y) , abs(min_Re.y));

            auto min_Im = modes.im->getMin();
            auto max_Im = modes.im->getMax();
            auto Δy_Im = max_Im.y - min_Im.y;
            fix range_Im = std::max(abs(max_Im.y) , abs(min_Im.y));

            fix range = std::max(range_Re, range_Im);
            fix Δy = std::max(Δy_Re, Δy_Im);

            mDFTGraph.set_yMin(-range - Δy * 0.15);
            mDFTGraph.set_yMax(+range + Δy * 0.15);
        }
    }


} // Tests