//
// Created by joao on 4/09/23.
//

#include "FourierTestWindow.h"

#include "Core/Tools/Log.h"

#include "Math/Function/NativeFunction.h"

#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"

#include "Math/Function/RtoR/Operations/DiscreteFourierTransform.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/InverseFourier.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/ComplexMagnitude.h"
#include "Math/Function/RtoC/FourierModes.h"
#include "Graphics/Plot2D/Plotter.h"
#include "Graphics/Plot2D/PlotThemeManager.h"

#define Re 0
#define Im 1

namespace Tests {
    using namespace Slab;

    using Plot = Graphics::Plotter;

    fix π = (float)Math::Constants::pi;

    float A = 1;
    float ω = 1;
    float ωₚₑₐₖ[2] = {0.0, ω};
    int N_modes = 10;
    float L = 2*π;
    fix ωₘₐₓ = 6*π;
    fix xMin = - .1*L;
    fix xMax =  1.1*L;

    float ω_0= 0.0;
    int decimation = 1;

    int N_sqrWave = 10;


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

    Math::Base::NativeFunction<Math::RtoR::Function> Func(func);
    Math::RtoR::InverseFourier FuncRebuilt;
    Math::RtoC::FourierModes fourierModes(Dummy(Func), 0, L, 100);
    Math::RtoR::ComplexMagnitude amplitudes(Dummy(fourierModes));

    FourierTestWindow::FourierTestWindow()
    : Graphics::WindowRow({"Fourier tests"})
    , theme(Graphics::PlotThemeManager::GetCurrent())
    , realFTArtist(nullptr, theme->funcPlotStyles[0])
    , imagFTArtist(nullptr, theme->funcPlotStyles[1])
    , loc1Artist(nullptr, theme->funcPlotStyles[3])
    , loc2Artist(nullptr, theme->funcPlotStyles[3])
    , funcArtist(nullptr, theme->funcPlotStyles[0], 2000)
    , rebuiltFuncArtist(nullptr, theme->funcPlotStyles[1], 2000)
    , gui()
    , mDFTGraph("DFT", gui.GetGUIContext())
    , mFuncGraph("func graph", gui.GetGUIContext())
    , mFTGraph("", gui.GetGUIContext())
    {
        updateGraphs();

        mFuncGraph.addArtist(Dummy(funcArtist));
        mFuncGraph.addArtist(Dummy(rebuiltFuncArtist));
        mFuncGraph.addArtist(Dummy(loc1Artist));
        mFuncGraph.addArtist(Dummy(loc2Artist));
        mFuncGraph.getRegion().setLimits(xMin, xMax, -3.25*A, 3.25*A);
        loc1Artist.setLabel("xₘᵢₙ");
        loc2Artist.setLabel("xₘₐₓ");

        mFTGraph.getRegion().setLimits(-1.1*ω, 1.1*ω, -0.1, 1.1);

        mDFTGraph.addArtist(Dummy(realFTArtist));
        mDFTGraph.addArtist(Dummy(imagFTArtist));

        Plot::AddRtoRFunction(Dummy(mFTGraph), Naked(amplitudes), theme->funcPlotStyles[4], Str("ℱ[") + funcSymbol + "](k)");

        addWindow(Dummy(gui), Graphics::WindowRow::Right, .25);

        col.addWindow(Dummy(mFTGraph));
        col.addWindow(Dummy(mDFTGraph));
        col.addWindow(Dummy(mFuncGraph));

        addWindow(Dummy(col));

        // static auto gui_ptr = Dummy(gui);
        // addResponder(gui_ptr);
    }

    void FourierTestWindow::draw() {
        gui.begin();
        gui.AddExternalDraw([this]() {

            if (ImGui::SliderFloat("ω", &ω, 0.1, ωₘₐₓ)
                | ImGui::SliderFloat("L", &L, 0.1, ωₘₐₓ)
                | ImGui::DragInt("N", &N_modes, (float) N_modes / 20.f, 1, 20000)
                | ImGui::DragInt("Nₛ", &N_sqrWave, 1, 1, 100)
                | ImGui::SliderFloat("ω₀", &ω_0, 0, ωₘₐₓ)
            | ImGui::SliderInt("dec", &decimation, 1, N_modes))
            updateGraphs();
        });

        gui.end();

        gui.addVolatileStat(Str("L = ") + ToStr(L / π, 2) + "π = " + ToStr(L));
        gui.addVolatileStat(Str("ω = ") + ToStr(ω / π, 2) + "π = " + ToStr(ω, 2));
        gui.addVolatileStat(Str("Re(ωₚₑₐₖ) = ") + ToStr(ωₚₑₐₖ[Re] / π, 2) + "π = " + ToStr(ωₚₑₐₖ[Re], 2));
        gui.addVolatileStat(Str("Im(ωₚₑₐₖ) = ") + ToStr(ωₚₑₐₖ[Im] / π, 2) + "π = " + ToStr(ωₚₑₐₖ[Im], 2));
        gui.addVolatileStat(Str("Re(ωₚₑₐₖ)/ω = ") + ToStr(ωₚₑₐₖ[Re] / ω, 2));
        gui.addVolatileStat(Str("Im(ωₚₑₐₖ)/ω = ") + ToStr(ωₚₑₐₖ[Im] / ω, 2));

        WindowRow::draw();
    }

    void FourierTestWindow::updateGraphs() {
        static Math::RtoR::DFTResult modes;
        using FFT = Math::RtoR::DFT;

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
                    ωₚₑₐₖ[Im] = (float)pt.x;
                }
            }

            auto style = theme->funcPlotStyles[1];
            style.lineColor.inverse();
            style.thickness = 2.5;
            style.setPrimitive(Slab::Graphics::VerticalLines);
            style.filled = false;
            style.lineColor.a = 0.8;


            realFTArtist.setPointSet(modes.re);
            realFTArtist.setLabel(Str("ℑ(ℱ[") + funcSymbol + "(x)])");

            style = theme->funcPlotStyles[3];
            style.lineColor.inverse();
            style.thickness = 2.5;
            style.setPrimitive(Slab::Graphics::VerticalLines);
            style.filled = false;
            style.lineColor.a = 0.8;
            imagFTArtist.setStyle(style);
            imagFTArtist.setPointSet(modes.im);
            imagFTArtist.setLabel(Str("ℜ(ℱ[") + funcSymbol + "(x)])");


            if (true) {
                mDFTGraph.getRegion().set_x_limits(-2 * π, 2*ωₘₐₓ);
            } else {
                auto min = modes.re->getMin();
                auto max = modes.re->getMax();
                auto Δx = max.x - min.x;

                mDFTGraph.getRegion().set_x_limits(min.x - Δx * 0.15, max.x + Δx * 0.15);
            }

            if (true) {
                mDFTGraph.getRegion().set_y_limits(1.25, -1.25);
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

                mDFTGraph.getRegion().set_y_limits(-range - Δy * 0.15, +range + Δy * 0.15);
            }
        }


        {
            auto style = theme->funcPlotStyles[0].permuteColors(true);
            style.filled = false;
            style.thickness = 5;
            funcArtist.setFunction(Dummy(Func));
            funcArtist.setLabel(funcSymbol);
            funcArtist.setStyle(style);

            style = theme->funcPlotStyles[1].permuteColors(true);
            style.filled = false;
            style.thickness = 5;
            FuncRebuilt.setModes(modes);
            rebuiltFuncArtist.setFunction(Dummy(FuncRebuilt));
            rebuiltFuncArtist.setLabel(Str("ℱ⁻¹[ℱ[")+funcSymbol+"(x)]]");
            rebuiltFuncArtist.setStyle(style);

            funcArtist.setSampling(N_modes);

            mFuncGraph.getRegion().set_x_limits(-0.1 * L, 1.1 * L);

            static Math::PointSet L_loc1, L_loc2;
            static bool initd = false;
            if (!initd) {
                L_loc1.setPoints({{0.0, -2},
                                  {0.0, +2}});

                loc1Artist.setPointSet(Dummy(L_loc1));
                loc2Artist.setPointSet(Dummy(L_loc2));

                initd = true;
            }

            L_loc2.setPoints({{L, -2},
                              {L, +2}});
        }
    }



} // Tests
