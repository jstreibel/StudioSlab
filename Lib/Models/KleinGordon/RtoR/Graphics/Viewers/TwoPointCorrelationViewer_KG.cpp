//
// Created by joao on 10/6/24.
//

#include "TwoPointCorrelationViewer_KG.h"
#include "Graphics/Plot2D/Plotter.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Math/Function/R2toC/R2toC_to_R2toR.h"
#include "Math/Function/R2toR/Calc/R2toRDFT.h"
#include "Graphics/DataViewers/Viewers/FourierViewer.h"
#include "Math/Function/RtoR/Model/Operators/Diff.h"

namespace Slab::Models::KGRtoR {

    using Plotter = Graphics::Plotter;
    using Themes = Graphics::PlotThemeManager;

    TwoPointCorrelationViewer_KG::TwoPointCorrelationViewer_KG(const Pointer<Graphics::GUIWindow> &guiWindow,
                                                               const Pointer<R2toR::NumericFunction> &func,
                                                               const Pointer<R2toR::NumericFunction> &ddtFunc)
    : KGViewer(guiWindow, func, ddtFunc)
    {
        twoPointArtist->setLabel("avg[ϕ(x)ϕ(x+r)]");

        auto twoPointWindow = New<PlottingWindow>("Two-point correlation");
        twoPointWindow->addArtist(twoPointArtist);
        addWindow(twoPointWindow);

        sectionArtist->setLabel("avg[ϕ(x)ϕ(x+r)] | r=(0,t)");
        sectionArtist->addSection(time_slice, Themes::GetCurrent()->funcPlotStyles[0].clone(), "x=0");
        sectionArtist->addSection(space_slice, Themes::GetCurrent()->funcPlotStyles[1].clone(), "t=0");
        auto timeSliceWindow = New<PlottingWindow>("Two-point time slice");
        timeSliceWindow->addArtist(sectionArtist);
        Plotter::AddRtoRFunction(timeSliceWindow, Naked(twoPointCorrelationAnalytic),
                                 Themes::GetCurrent()->funcPlotStyles[1], "Analytic, x=0");
        Plotter::AddRtoRFunction(timeSliceWindow, Naked(powerDecayCorrelation),
                                 Themes::GetCurrent()->funcPlotStyles[1], "Analytic, t=0");
        addWindow(timeSliceWindow, true);
        ddt2ptSection = Plotter::AddRtoRFunction(timeSliceWindow, section_map, Themes::GetCurrent()->funcPlotStyles[2], "Section also", 1000);

    }

    void TwoPointCorrelationViewer_KG::setFunction(Pointer<Math::R2toR::NumericFunction> function) {
        Viewer::setFunction(function);

        {
            fix t_min = (float)function->getDomain().yMin;
            fix t_max = (float)function->getDomain().yMax;
            t0 = t_min;
            Δt = t_max-t_min;

            // two-point correlation domain is always centered at (0,0);
            fix tcorr_min = -.5*Δt;
            fix tcorr_max =  .5*Δt;
            fix delta_tcorr = tcorr_max-tcorr_min;

            auto &x0 = time_slice->getx0();
            x0.x = .0;
            x0.y = tcorr_min;

            auto &r = time_slice->getr();
            r.x = .0;
            r.y = delta_tcorr;

            time_slice->set_s(tcorr_min, tcorr_max);
        }
        {
            fix hlx = .5*function->getDomain().getLx();

            auto &x0 = space_slice->getx0();
            x0.x = -hlx;
            x0.y = .0;

            auto &r = space_slice->getr();
            r.x = 2.*hlx;
            r.y = .0;
            space_slice->set_s(-hlx, hlx);
        }
    }

    void TwoPointCorrelationViewer_KG::draw() {
        auto function = getFunction();

        fix c0_min=1e-5f;
        fix c0_max=1e5f;
        fix xi_min = 1e-3f;
        fix xi_max = 1e+3f;
        fix lambda_min = 1e-2f;
        fix lambda_max = 1e+2f;

        beginGUI();
        ImGui::SeparatorText("Space-domain analytic 2-point correlation");
        {
            auto &funky = powerDecayCorrelation;
            ImGui::BeginGroup();
            {
                using Nature = CorrelationDecay::Nature;
                Nature nature = funky.getNature();

                if (ImGui::RadioButton("Exponential##spacedomain", nature == Nature::Exponential)) {
                    funky.setNature(CorrelationDecay::Exponential);
                }
                if (ImGui::RadioButton("Power##spacedomain", nature == Nature::Power)) {
                    funky.setNature(CorrelationDecay::Power);
                }
            }
            ImGui::EndGroup();

            auto c0 = (float) funky.get_c0();
            auto xi = (float) (funky.getξ());
            auto beta = (float) funky.getβ();

            if (ImGui::SliderFloat("c₀##spacedomain", &c0, c0_min, c0_max)
             |  ImGui::DragFloat("c₀##spacedomain:drag", &c0, c0 / 1000, c0_min, c0_max, "%.5f")) {
                funky.set_c0(c0);
            }
            if (ImGui::SliderFloat("β##spacedomain", &beta, 1.01, 4)) {
                funky.setβ(beta);
            }
            if (ImGui::SliderFloat("ξ##spacedomain", &xi, xi_min, xi_max)
             |  ImGui::DragFloat("ξ##spacedomain:drag", &xi, xi / 1000, xi_min, xi_max, "%.3f")) {
                funky.setξ(xi);
            }
        }

        ImGui::SeparatorText("Time-domain analytic 2-point correlation");
        {
            auto &funky = twoPointCorrelationAnalytic;

            ImGui::BeginGroup();
            {
                using Nature = CorrelationDecay::Nature;
                Nature nature = funky.getNature();

                if (ImGui::RadioButton("Exponential##timedomain", nature == Nature::Exponential)) {
                    funky.setNature(CorrelationDecay::Exponential);
                }
                if (ImGui::RadioButton("Power##timedomain", nature == Nature::Power)) {
                    funky.setNature(CorrelationDecay::Power);
                }
            }
            ImGui::EndGroup();


            auto c0 = (float) funky.get_c0();
            auto lambda = (float) funky.getλ();
            auto xi = (float) funky.getξ();
            auto beta = (float) funky.getβ();

            if (ImGui::SliderFloat("c₀##timedomain", &c0, c0_min, c0_max)
                | ImGui::DragFloat("c₀##timedomain:drag", &c0, c0 / 1000, c0_min, c0_max, "%.5f")) {
                funky.set_c0(c0);
            }
            if (ImGui::SliderFloat("λ##timedomain:analytic", &lambda, lambda_min, lambda_max)
              | ImGui::DragFloat("λ##timedomain:analytic:drag", &lambda, lambda_min, lambda_max, lambda/1000, "%.4f") ) {
                funky.setλ(lambda);
            }
            if (ImGui::SliderFloat("β##timedomain", &beta, 1.1, 4)) {
                funky.setβ(beta);
            }
            if (ImGui::SliderFloat("ξ##timedomain", &xi, xi_min, xi_max)
              | ImGui::DragFloat("ξ##timedomain:drag", &xi, xi / 1000, xi_min, xi_max, "%.3f")) {
                funky.setξ(xi);
            }
        }

        ImGui::SeparatorText("Numeric 2-point correlation");
        if(function!= nullptr) {
            fix t_min = (float) function->getDomain().yMin;
            fix t_max = (float) function->getDomain().yMax;
            fix dt = function->getSpace().getMetaData().geth(1);

            if (ImGui::SliderFloat("tₘᵢₙ", &t0, t_min, t_max - Δt)
                | ImGui::SliderFloat("Δt", &Δt, 10 * (float) dt, t_max - t0)) {
                if (auto_update_Ftx) computeTwoPointCorrelation();
            }

            ImGui::Checkbox("Auto compute##2pt_corr", &auto_update_Ftx);
            if(ImGui::Button("Compute <ϕ(x)ϕ(x+r)>"))
                computeTwoPointCorrelation();
        }

        endGUI();

        WindowPanel::draw();
    }

    void TwoPointCorrelationViewer_KG::computeTwoPointCorrelation() {
        auto function = getFunction();
        if(function == nullptr) return;

        // Fourier transform *********************************************************************************
        Real t_0 = (Real)t0;
        Real t_f = t_0 + (Real)Δt;
        auto toFT = Graphics::FourierViewer::FilterSpace(function, t_0, t_f);
        auto dft2DFunction = Math::R2toR::R2toRDFT::DFTReal(*toFT);

        // Power spectrum ************************************************************************************
        auto powerSpectrum = Math::Convert(dft2DFunction, Math::PowerSpectrum);

        // Two-point correlation *****************************************************************************
        twoPointFunction = Math::Convert(Math::R2toR::R2toRDFT::DFTReal(*powerSpectrum,
                                                          R2toR::R2toRDFT::InverseFourier,
                                                          R2toR::R2toRDFT::Auto, R2toR::R2toRDFT::Mangle),
                                         Math::RealPart);

        // Setup plots
        twoPointArtist->setFunction(twoPointFunction);
        sectionArtist->setFunction(twoPointFunction);
        *section_map = Section(twoPointFunction, time_slice);
        using Diff = Math::RtoR::Diff;
        auto diff = New<Diff>(section_map, twoPointFunction->getSpace().getMetaData().geth(1)*1.2);
        *ddt2ptSection = Graphics::RtoRFunctionArtist(diff, Themes::GetCurrent()->funcPlotStyles[3], 1000);

    }

    Str TwoPointCorrelationViewer_KG::getName() const {
        return "[KG] 2-point correlation viewer";
    }
} // Slab::Models::KGRtoR