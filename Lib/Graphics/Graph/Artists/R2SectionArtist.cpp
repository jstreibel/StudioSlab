//
// Created by joao on 22/05/24.
//

#include "R2SectionArtist.h"
#include "Graphics/OpenGL/LegacyGL/FunctionRenderer.h"

#include "Graphics/Graph/PlottingWindow.h"

#include <utility>
#include "3rdParty/ImGui.h"

namespace Slab::Graphics {
    void R2SectionArtist::setFunction(R2toR::Function_constptr func)  { function2D = std::move(func); }

    void R2SectionArtist::addSection(const RtoR2::ParametricCurve_ptr& section, const Pointer<PlotStyle>& style, const Str& name) {
        sections.emplace_back(section, style, name);
    }

    bool R2SectionArtist::draw(const PlottingWindow &window) {
        if(function2D == nullptr) return true;

        for (const auto& section_data : sections) {
            if(!section_data.visible) continue;

            auto section = section_data.section;
            auto style = section_data.style;
            auto name = section_data.name;

            FunctionRenderer::renderSection(*function2D, *section, *style, samples);

            window.requireLabelOverlay(name, style);
        }

        return true;

    }

    void R2SectionArtist::setSamples(Resolution res) { samples = res; }

    auto R2SectionArtist::getFunction() const -> R2toR::Function_constptr {
        return function2D;
    }

    bool R2SectionArtist::hasGUI() { return true; }

    void R2SectionArtist::drawGUI() {
        auto samples_int = (int)samples;
        if(ImGui::SliderInt(UniqueName("Samples").c_str(), &samples_int, 100, 25000)){
            samples = (Resolution)samples_int;
        }

        int i=0;
        for(auto &section_data : sections) {
            auto style = section_data.style;
            ImGui::SeparatorText((Str("Section ") + ToStr(++i)).c_str());

            ImGui::Checkbox((UniqueName("Visible")+section_data.name).c_str(), &section_data.visible);

            ImGui::SliderFloat("Line thickness", &style->thickness, 0.1, 25);

            static bool colorPickerActive = false;
            if(ImGui::Button("Line color"))
                colorPickerActive = !colorPickerActive;

            auto uniqueName = this->getLabel() + Str(" line color picker");

            if(colorPickerActive && ImGui::Begin(uniqueName.c_str(), &colorPickerActive)) {
                ImGui::ColorPicker4("Line color", style->lineColor.asFloat4fv(), 0);
                ImGui::End();
            }

            ImGui::Checkbox("Filled", &style->filled);
            if(style->filled) {
                static bool fillColorPickerActive = false;
                if(ImGui::Button("Fill color"))
                    fillColorPickerActive = !fillColorPickerActive;

                uniqueName = this->getLabel() + Str(" fill color picker");
                if(fillColorPickerActive && ImGui::Begin(uniqueName.c_str(), &fillColorPickerActive)){
                    ImGui::ColorPicker4("Fill color", style->fillColor.asFloat4fv(), 0);
                    ImGui::End();
                }
            }
        }
    }

    void R2SectionArtist::clearSections() {
        sections.clear();
    }

    const RectR &R2SectionArtist::getRegion() {
        if(sections.empty() || function2D==nullptr) return Artist::getRegion();

        auto s_min = sections[0].section->get_sMin();
        auto s_max = sections[0].section->get_sMax();

        auto val_min =  Infinity;
        auto val_max = -Infinity;

        for(const auto& section : sections) {
            for (int i = 0; i < samples; ++i) {
                auto s = s_min + (i / (Real) samples) * (s_max - s_min);

                auto r = (*section.section)(s);
                auto val = (*function2D)(r);

                if(val<val_min) val_min=val;
                if(val>val_max) val_max=val;
            }
        }

        region = {s_min, s_max, val_min, val_max};

        return Artist::getRegion();
    }
} // Graphics