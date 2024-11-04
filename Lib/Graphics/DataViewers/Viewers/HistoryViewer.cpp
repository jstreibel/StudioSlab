//
// Created by joao on 8/11/24.
//

#include "3rdParty/ImGui.h"

#include "HistoryViewer.h"

#include "Graphics/Plot2D/Plotter.h"
#include "Graphics/Plot2D/PlotThemeManager.h"

#include "Math/Function/R2toR/Calc/R2toRDFT.h"
#include "Math/Function/R2toC/R2toC_to_R2toR.h"

namespace Slab::Graphics {

    HistoryViewer::HistoryViewer(const Pointer<GUIWindow> &gui_window, const Pointer<R2toR::NumericFunction> &function)
    : Viewer(gui_window, function)
    {
        history_window = New<Plot2DWindow>("Function", gui_window->GetGUIContext());
        function_artist = Plotter::AddR2toRFunction(history_window, nullptr, "ϕ(t,x)");
        addWindow(history_window);

        xft_history_window = New<Plot2DWindow>("Space DFT", gui_window->GetGUIContext());
        xft_amplitudes_artist = Plotter::AddR2toRFunction(xft_history_window, nullptr, "ℱₓ[ϕ]");
        addWindow(xft_history_window, true);

        auto slice_window = New<Plot2DWindow>("Space instant", gui_window->GetGUIContext());
        section_artist = Plotter::AddR2Section(slice_window, nullptr, "ϕ");
        addWindowToColumn(slice_window, 0);

        auto dft_slice_window = New<Plot2DWindow>("Space DFT instant", gui_window->GetGUIContext());
        dft_section_artist = Plotter::AddR2Section(dft_slice_window, nullptr, "ℱₓ[ϕ]");
        addWindowToColumn(dft_slice_window, 1);

        history_window->tieRegion_xMaxMin(*slice_window);

        xft_history_window->tieRegion_xMaxMin(*dft_slice_window);
        xft_history_window->tieRegion_yMaxMin(*history_window);


    }

    void HistoryViewer::setFunction(Pointer<Math::R2toR::NumericFunction> function) {
        Viewer::setFunction(function);

        auto funky = getFunction();
        function_artist->setFunction(funky);
        section_artist->setFunction(funky);

        auto domain = funky->getDomain();
        if(function_section == nullptr) {
            function_section = New<RtoR2::StraightLine>(Real2D{domain.xMin, domain.yMin},
                                                        Real2D{domain.xMax, domain.yMin},
                                                        domain.xMin, domain.xMax);
            auto style = PlotThemeManager::GetCurrent()->funcPlotStyles[0].clone();
            style->filled = false;
            section_artist->addSection(function_section, style, "");

            auto history_section_artist = Plotter::AddCurve(history_window, function_section, *style, "History section");
            xft_history_window->addArtist(history_section_artist);
        } else {
            function_section->getx0().x = domain.xMin;
            function_section->getr().x = domain.getLx();
            function_section->set_s(domain.xMin, domain.xMax);
        }

        section_artist->setSamples(funky->getN()*oversampling);

        xFourierTransform = nullptr;

        if(this->isVisible()) computeTransform();
    }

    void HistoryViewer::notifyBecameVisible() {
        Viewer::notifyBecameVisible();

        if(xFourierTransform == nullptr) computeTransform();
    }

    void HistoryViewer::computeTransform() {
        auto func = getFunction();
        if(func == nullptr) return;

        xFourierTransform = Math::R2toR::R2toRDFT::SpaceDFTReal(*func);

        xft_amplitudes = Math::Convert(xFourierTransform, Math::R2toC_to_R2toR_Mode::Magnitude);

        xft_amplitudes_artist->setFunction(xft_amplitudes);
        dft_section_artist->setFunction(xft_amplitudes);

        auto domain = xft_amplitudes->getDomain();
        if(dft_section == nullptr) {
            dft_section = New<RtoR2::StraightLine>(Real2D{domain.xMin, domain.yMin},
                                                   Real2D{domain.xMax, domain.yMin},
                                                   domain.xMin, domain.xMax);
            auto style = PlotThemeManager::GetCurrent()->funcPlotStyles[1];
            dft_section_artist->addSection(dft_section, style.clone(), "dft section");
        } else {
            dft_section->getx0().x = domain.xMin;
            dft_section->getr().x = domain.getLx();
            dft_section->set_s(domain.xMin, domain.xMax);
        }

        dft_section_artist->setSamples(func->getN()*oversampling);
    }

    void HistoryViewer::draw() {
        auto func = getFunction();

        if(func== nullptr) return;

        beginGUI();
        fix t_min = (float)func->getDomain().yMin;
        fix t_max = (float)func->getDomain().yMax;
        auto current_t = (float)curr_t;
        if(ImGui::SliderFloat("t##HistoryViewer", &current_t, t_min, t_max)){
            curr_t = (Real)current_t;
            function_section->getx0().y = dft_section->getx0().y = curr_t;
            // function_section->getr().y = dft_section->getr().y = curr_t;
        }

        if(ImGui::SliderInt("oversampling", &oversampling, 1, 16)) {
            section_artist->setSamples(func->getN()*oversampling);
            dft_section_artist->setSamples(xft_amplitudes->getN()*oversampling);
        }

        endGUI();

        WindowPanel::draw();
    }

    Str HistoryViewer::getName() const {
        return "History viewer";
    }

} // Slab::Graphics