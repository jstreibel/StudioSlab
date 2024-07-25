//
// Created by joao on 7/24/24.
//

#include "VShapeExpansionTest.h"

#include "Graphics/Graph/Plotter.h"
#include "Graphics/Graph/PlotThemeManager.h"

namespace Tests {
    using namespace Slab;

    VShapeExpansionTest::VShapeExpansionTest() : Graphics::WindowRow(HasMainMenu) {
        potential = New<Slab::Math::RtoR::NonlinearKGPotential>(1.0, 43, 1.0);
        derivs = potential->diff(0);

        auto window = New<Graphics::PlottingWindow>("V-shape series test");
        auto style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[0];
        style.filled = false;
        Graphics::Plotter::AddRtoRFunction(window, potential, style, "KG V-shape");
        Graphics::Plotter::AddRtoRFunction(window, derivs, style, "V'");

        addWindow(Naked(stats), Right, 0.15);
        addWindow(window);
    }

    void VShapeExpansionTest::draw() {
        stats.begin();

        auto A = (float)potential->getA();
        auto s = (float)potential->get_s();
        auto N = (int)potential->getN();

        if(ImGui::SliderFloat("A₀", &A, 0.1, 10)) potential->setA ((Real)A);
        if(ImGui::SliderFloat("s",  &s,-3,    3)) potential->set_s((Real)s);
        if(ImGui::SliderInt("N", &N, 1, 55, "%d")) {
            if(!(N%2)) --N;

            potential->setN(N);
        }

        stats.end();

        WindowRow::draw();
    }
} // Tests