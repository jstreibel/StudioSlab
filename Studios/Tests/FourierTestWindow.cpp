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

    DevFloat func(const DevFloat x) {
        if(0) {
            DevFloat val = 0.0;
            for (int i = 0; i < N_sqrWave; ++i) {
                int n = 2 * i + 1;
                val += sin(n * ω * x) / (DevFloat) n;
            }
            return 2 * val / π;
        } else return A*sin(ω*x);
    }

    Math::Base::NativeFunction<Math::RtoR::Function> Func(func);
    Math::RtoR::InverseFourier FuncRebuilt;
    Math::RtoC::FourierModes fourierModes(Dummy(Func), 0, L, 100);
    Math::RtoR::ComplexMagnitude amplitudes(Dummy(fourierModes));

    FourierTestWindow::FourierTestWindow()
    : Graphics::FWindowRow({"Fourier tests"})
    , theme(Graphics::PlotThemeManager::GetCurrent())
    , realFTArtist(nullptr, theme->FuncPlotStyles[0])
    , imagFTArtist(nullptr, theme->FuncPlotStyles[1])
    , loc1Artist(nullptr, theme->FuncPlotStyles[3])
    , loc2Artist(nullptr, theme->FuncPlotStyles[3])
    , funcArtist(nullptr, theme->FuncPlotStyles[0], 2000)
    , rebuiltFuncArtist(nullptr, theme->FuncPlotStyles[1], 2000)
    , gui(Graphics::FSlabWindowConfig{"GUI"})
    , mDFTGraph("DFT")
    , mFuncGraph("func graph")
    , mFTGraph("")
    {
        updateGraphs();

        mFuncGraph.AddArtist(Dummy(funcArtist));
        mFuncGraph.AddArtist(Dummy(rebuiltFuncArtist));
        mFuncGraph.AddArtist(Dummy(loc1Artist));
        mFuncGraph.AddArtist(Dummy(loc2Artist));
        mFuncGraph.GetRegion().setLimits(xMin, xMax, -3.25*A, 3.25*A);
        loc1Artist.SetLabel("xₘᵢₙ");
        loc2Artist.SetLabel("xₘₐₓ");

        mFTGraph.GetRegion().setLimits(-1.1*ω, 1.1*ω, -0.1, 1.1);

        mDFTGraph.AddArtist(Dummy(realFTArtist));
        mDFTGraph.AddArtist(Dummy(imagFTArtist));

        Plot::AddRtoRFunction(Dummy(mFTGraph), Naked(amplitudes), theme->FuncPlotStyles[4], Str("ℱ[") + funcSymbol + "](k)");

        AddWindow(Dummy(gui), Graphics::FWindowRow::Right, .25);

        col.addWindow(Dummy(mFTGraph));
        col.addWindow(Dummy(mDFTGraph));
        col.addWindow(Dummy(mFuncGraph));

        AddWindow(Dummy(col));

        // static auto gui_ptr = Dummy(gui);
        // addResponder(gui_ptr);
    }

    void FourierTestWindow::ImmediateDraw(const Graphics::FPlatformWindow& PlatformWindow) {
        gui.AddExternalDraw([this]() {

            if (ImGui::SliderFloat("ω", &ω, 0.1, ωₘₐₓ)
                | ImGui::SliderFloat("L", &L, 0.1, ωₘₐₓ)
                | ImGui::DragInt("N", &N_modes, (float) N_modes / 20.f, 1, 20000)
                | ImGui::DragInt("Nₛ", &N_sqrWave, 1, 1, 100)
                | ImGui::SliderFloat("ω₀", &ω_0, 0, ωₘₐₓ)
            | ImGui::SliderInt("dec", &decimation, 1, N_modes))
            updateGraphs();
        });

        gui.AddVolatileStat(Str("L = ") + ToStr(L / π, 2) + "π = " + ToStr(L));
        gui.AddVolatileStat(Str("ω = ") + ToStr(ω / π, 2) + "π = " + ToStr(ω, 2));
        gui.AddVolatileStat(Str("Re(ωₚₑₐₖ) = ") + ToStr(ωₚₑₐₖ[Re] / π, 2) + "π = " + ToStr(ωₚₑₐₖ[Re], 2));
        gui.AddVolatileStat(Str("Im(ωₚₑₐₖ) = ") + ToStr(ωₚₑₐₖ[Im] / π, 2) + "π = " + ToStr(ωₚₑₐₖ[Im], 2));
        gui.AddVolatileStat(Str("Re(ωₚₑₐₖ)/ω = ") + ToStr(ωₚₑₐₖ[Re] / ω, 2));
        gui.AddVolatileStat(Str("Im(ωₚₑₐₖ)/ω = ") + ToStr(ωₚₑₐₖ[Im] / ω, 2));

        FWindowRow::ImmediateDraw(PlatformWindow);
    }

    void FourierTestWindow::updateGraphs() {
        static Math::RtoR::DFTResult modes;
        using FFT = Math::RtoR::DFT;

        fourierModes.setL(L);
        fourierModes.setNSamples(N_modes);

        {

            modes = FFT::Compute(Func, N_modes, 0, L);

            ωₚₑₐₖ[Re] = 0.0;
            DevFloat A_max = 0.0;
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

            auto style = theme->FuncPlotStyles[1];
            style.lineColor.inverse();
            style.thickness = 2.5;
            style.setPrimitive(Slab::Graphics::VerticalLines);
            style.filled = false;
            style.lineColor.a = 0.8;


            realFTArtist.setPointSet(modes.re);
            realFTArtist.SetLabel(Str("ℑ(ℱ[") + funcSymbol + "(x)])");

            style = theme->FuncPlotStyles[3];
            style.lineColor.inverse();
            style.thickness = 2.5;
            style.setPrimitive(Slab::Graphics::VerticalLines);
            style.filled = false;
            style.lineColor.a = 0.8;
            imagFTArtist.setStyle(style);
            imagFTArtist.setPointSet(modes.im);
            imagFTArtist.SetLabel(Str("ℜ(ℱ[") + funcSymbol + "(x)])");


            if (true) {
                mDFTGraph.GetRegion().set_x_limits(-2 * π, 2*ωₘₐₓ);
            } else {
                auto min = modes.re->getMin();
                auto max = modes.re->getMax();
                auto Δx = max.x - min.x;

                mDFTGraph.GetRegion().set_x_limits(min.x - Δx * 0.15, max.x + Δx * 0.15);
            }

            if (true) {
                mDFTGraph.GetRegion().set_y_limits(1.25, -1.25);
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

                mDFTGraph.GetRegion().set_y_limits(-range - Δy * 0.15, +range + Δy * 0.15);
            }
        }


        {
            auto style = theme->FuncPlotStyles[0].permuteColors(true);
            style.filled = false;
            style.thickness = 5;
            funcArtist.setFunction(Dummy(Func));
            funcArtist.SetLabel(funcSymbol);
            funcArtist.setStyle(style);

            style = theme->FuncPlotStyles[1].permuteColors(true);
            style.filled = false;
            style.thickness = 5;
            FuncRebuilt.setModes(modes);
            rebuiltFuncArtist.setFunction(Dummy(FuncRebuilt));
            rebuiltFuncArtist.SetLabel(Str("ℱ⁻¹[ℱ[")+funcSymbol+"(x)]]");
            rebuiltFuncArtist.setStyle(style);

            funcArtist.setSampling(N_modes);

            mFuncGraph.GetRegion().set_x_limits(-0.1 * L, 1.1 * L);

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
