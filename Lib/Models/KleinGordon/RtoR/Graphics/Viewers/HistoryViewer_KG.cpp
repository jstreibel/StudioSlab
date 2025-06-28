//
// Created by joao on 8/11/24.
//

#include "3rdParty/ImGui.h"

#include "HistoryViewer_KG.h"

#include "Graphics/Plot2D/Plotter.h"
#include "Graphics/Plot2D/PlotThemeManager.h"

#include "Math/Function/R2toR/Calc/R2toRDFT.h"
#include "Math/Function/R2toC/R2toC_to_R2toR.h"

namespace Slab::Models::KGRtoR {

    HistoryViewer::HistoryViewer(const Pointer<Graphics::GUIWindow> &gui_window)
    : KGViewer(gui_window)
    {
        history_window = New<Graphics::Plot2DWindow>("Function");
        function_artist = Graphics::Plotter::AddR2toRFunction(history_window, nullptr, "ϕ(t,x)");
        ddt_function_artist = Graphics::Plotter::AddR2toRFunction(history_window, nullptr, "ϕₜ(t,x)");
        d2dt2_function_artist = Graphics::Plotter::AddR2toRFunction(history_window, nullptr, "ϕₜₜ(t,x)");
        addWindow(history_window);

        slice_window = New<Graphics::Plot2DWindow>("Slices");
        function_section_artist = Graphics::Plotter::AddR2Section(slice_window, nullptr, "ϕ");
        ddt_function_section_artist = Graphics::Plotter::AddR2Section(slice_window, nullptr, "ϕₜ");
        d2dt2_function_section_artist = Graphics::Plotter::AddR2Section(slice_window, nullptr, "ϕₜₜ");

        addWindowToColumn(slice_window, 0);

        history_window->tieRegion_xMaxMin(*slice_window);
    }

    void HistoryViewer::draw() {
        if(getFunction() == nullptr) return;

        gui_window->AddExternalDraw([this](){
            auto func = getFunction();

            fix t_min = (float)func->getDomain().yMin;
            fix t_max = (float)func->getDomain().yMax;
            auto current_t = (float)(curr_ti*dt);
            if(ImGui::SliderFloat("t##HistoryViewer", &current_t, t_min, t_max)){
                curr_ti = floor(current_t/dt);
                function_section->getx0().y = current_t;
            }
            if(ImGui::DragInt("tᵢ##HistoryViewer(2)", &curr_ti, 1e-1f, 0, getFunction()->getM())){
                function_section->getx0().y = curr_ti*dt;
            }

            if(ImGui::SliderInt("oversampling", &oversampling, 1, 16)) {
                function_section_artist->setSamples(func->getN() * oversampling);
            }
        });

        WindowPanel::draw();
    }

    void HistoryViewer::setFunction(Pointer<Math::R2toR::NumericFunction> function) {
        Viewer::setFunction(function);

        dt = function->getDomain().getLy() / (DevFloat)function->getM();

        auto funky = getFunction();
        function_artist->setFunction(funky);
        function_section_artist->setFunction(funky);

        auto domain = funky->getDomain();
        if(function_section == nullptr) {
            function_section = New<RtoR2::StraightLine>(Real2D{domain.xMin, domain.yMin},
                                                        Real2D{domain.xMax, domain.yMin},
                                                        domain.xMin, domain.xMax);
            auto style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[0].clone();
            style->filled = false;
            function_section_artist->addSection(function_section, style, "ϕ");

            auto history_section_artist = Graphics::Plotter::AddCurve(history_window, function_section, *style, "History section");

        } else {
            function_section->getx0().x = domain.xMin;
            function_section->getr().x = domain.getLx();
            function_section->set_s(domain.xMin, domain.xMax);
        }

        function_section_artist->setSamples(funky->getN() * oversampling);
    }

    void HistoryViewer::setFunctionDerivative(FuncPointer pointer) {
        KGViewer::setFunctionDerivative(pointer);

        auto funky = getFunctionDerivative();
        d2dt2_function = DynamicPointerCast<R2toR::NumericFunction>(funky->diff(1));

        ddt_function_artist->setFunction(funky);
        ddt_function_section_artist->setFunction(funky);
        ddt_function_section_artist->clearSections();

        d2dt2_function_artist->setFunction(d2dt2_function);
        d2dt2_function_section_artist->setFunction(d2dt2_function);
        d2dt2_function_section_artist->clearSections();

        auto style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[1].clone();
        ddt_function_section_artist->addSection(function_section, style, "ϕₜ");

        style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[2].clone();
        d2dt2_function_section_artist->addSection(function_section, style, "ϕₜₜ");
    }

    Str HistoryViewer::getName() const {
        return "[KG] History viewer";
    }

} // Slab::Graphics