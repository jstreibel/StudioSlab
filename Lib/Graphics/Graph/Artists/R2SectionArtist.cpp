//
// Created by joao on 22/05/24.
//

#include "R2SectionArtist.h"
#include "Graphics/OpenGL/LegacyGL/FunctionRenderer.h"

#include <utility>

namespace Graphics {
    void R2SectionArtist::setFunction(R2toR::Function_ptr func)  { function2D = std::move(func); }

    void R2SectionArtist::addSection(const RtoR2::ParametricCurve_ptr& section, PlotStyle style) {
        sections.emplace_back(section, style);
    }

    void R2SectionArtist::draw(const PlottingWindow &window) {
        if(function2D == nullptr) return;

        for (const auto& pair : sections) {
            auto section = pair.first.get();
            auto style = pair.second;

            FunctionRenderer::renderSection(*function2D, *section, style, samples);
        }

    }

    void R2SectionArtist::setSamples(Resolution res) { samples = res; }

    auto R2SectionArtist::getFunction() const -> R2toR::Function_constptr {
        return function2D;
    }
} // Graphics