//
// Created by joao on 22/05/24.
//

#include "R2SectionArtist.h"
#include "Graphics/OpenGL/LegacyGL/FunctionRenderer.h"

#include <utility>
#include "3rdParty/ImGui.h"

namespace Slab::Graphics {
    void R2SectionArtist::setFunction(R2toR::Function_constptr func)  { function2D = std::move(func); }

    void R2SectionArtist::addSection(const RtoR2::ParametricCurve_ptr& section, PlotStyle style) {
        sections.emplace_back(section, style);
    }

    bool R2SectionArtist::draw(const PlottingWindow &window) {
        if(function2D == nullptr) return true;

        for (const auto& pair : sections) {
            auto section = pair.first.get();
            auto style = pair.second;

            FunctionRenderer::renderSection(*function2D, *section, style, samples);
        }

        return true;

    }

    void R2SectionArtist::setSamples(Resolution res) { samples = res; }

    auto R2SectionArtist::getFunction() const -> R2toR::Function_constptr {
        return function2D;
    }

    bool R2SectionArtist::hasGUI() { return true; }

    void R2SectionArtist::drawGUI() {
        int i=0;
        for(auto &pCurve : sections) {
            auto &style = pCurve.second;
            ImGui::Text("%s%i", "Section ", ++i);

            ImGui::SliderFloat("Line thickness", &style.thickness, 0.1, 25);

            static bool colorPickerActive = false;
            if(ImGui::Button("Line color"))
                colorPickerActive = !colorPickerActive;

            auto uniqueName = this->getLabel() + Str(" line color picker");

            if(colorPickerActive && ImGui::Begin(uniqueName.c_str(), &colorPickerActive)) {
                ImGui::ColorPicker4("Line color", style.lineColor.asFloat4fv(), 0);
                ImGui::End();
            }

            ImGui::Checkbox("Filled", &style.filled);
            if(style.filled) {
                static bool fillColorPickerActive = false;
                if(ImGui::Button("Fill color"))
                    fillColorPickerActive = !fillColorPickerActive;

                uniqueName = this->getLabel() + Str(" fill color picker");
                if(fillColorPickerActive && ImGui::Begin(uniqueName.c_str(), &fillColorPickerActive)){
                    ImGui::ColorPicker4("Fill color", style.fillColor.asFloat4fv(), 0);
                    ImGui::End();
                }
            }
        }
    }
} // Graphics