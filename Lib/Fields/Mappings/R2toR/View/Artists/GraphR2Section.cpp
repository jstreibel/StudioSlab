//
// Created by joao on 10/11/22.
//

#include "GraphR2Section.h"
#include "Fields/Mappings/FunctionRenderer.h"


GraphR2Section::GraphR2Section(double xMin, double xMax, double yMin, double yMax, String title, bool filled,
                               int samples) : Graph(xMin, xMax, yMin, yMax, title, filled, samples) {}

void GraphR2Section::_renderFunction(const R2toR::Function *func, Color color) {

    for(auto pair : sections) {
        auto section = pair.first;
        //auto color = pair.second;

        RtoR::FunctionRenderer::renderSection(*func, *section, color, filled, samples);
    }
}

void GraphR2Section::addSection(RtoR2::StraightLine *section, Color color) {
    sections.emplace_back(std::pair{section, color});
}

void GraphR2Section::__drawLabel() {

}

