//
// Created by joao on 8/11/24.
//

#include "3rdParty/ImGui.h"

#include "ModesHistoryViewer.h"

#include "Graphics/Graph/Plotter.h"
#include "Graphics/Graph/PlotThemeManager.h"

#include "Math/Function/R2toR/Calc/R2toRDFT.h"
#include "Math/Function/R2toC/R2toC_to_R2toR.h"

namespace Slab::Graphics {

    ModesHistoryViewer::ModesHistoryViewer(const Pointer<GUIWindow> &guiWindow)
    : Viewer(guiWindow, nullptr)
    {
        xft_history_window = New<PlottingWindow>("Space DFT");
        xft_amplitudes_artist = Plotter::AddR2toRFunction(xft_history_window, nullptr, "ℱₓ[ϕ]");
        addWindow(xft_history_window);

        modes_window = New<PlottingWindow>("Modes");
        addWindow(modes_window);
    }

    void ModesHistoryViewer::draw() {
        if(getFunction() == nullptr) return;

        beginGUI();
        fix dk = (float) (M_PI/getFunction()->getDomain().getLx());
        fix k_min = dk;
        fix k_max = dk * (float)getFunction()->getN();
        if(ImGui::SliderFloat("base mode", &base_mode, k_min, k_max)) {

        }
        endGUI();

        WindowPanel::draw();
    }

    void ModesHistoryViewer::setFunction(Pointer<Math::R2toR::NumericFunction> function) {
        Viewer::setFunction(function);

        xFourierTransform = nullptr;

        if(this->isVisible()) computeTransform();
    }

    void ModesHistoryViewer::computeTransform() {
        auto func = getFunction();
        if(func == nullptr) return;

        xFourierTransform = Math::R2toR::R2toRDFT::SpaceDFTReal(*func);

        xft_amplitudes = Math::Convert(xFourierTransform, Math::R2toC_to_R2toR_Mode::Magnitude);

        xft_amplitudes_artist->setFunction(xft_amplitudes);
        // dft_section_artist->setFunction(xft_amplitudes);

        auto domain = xft_amplitudes->getDomain();
        // if(dft_section == nullptr) {
        //     dft_section = New<RtoR2::StraightLine>(Real2D{domain.xMin, domain.yMin},
        //                                            Real2D{domain.xMax, domain.yMin},
        //                                            domain.xMin, domain.xMax);
        //     auto style = PlotThemeManager::GetCurrent()->funcPlotStyles[1];
        //     dft_section_artist->addSection(dft_section, style);
        // } else {
        //     dft_section->getx0().x = domain.xMin;
        //     dft_section->getr().x = domain.getLx();
        //     dft_section->set_s(domain.xMin, domain.xMax);
        // }

        // dft_section_artist->setSamples(func->getN()*oversampling);
    }

    void ModesHistoryViewer::notifyBecameVisible() {
        Viewer::notifyBecameVisible();

        if(xFourierTransform == nullptr) computeTransform();
    }

} // Slab::Graphics