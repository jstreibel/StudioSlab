//
// Created by joao on 7/24/24.
//

#include "VShapeExpansionTest.h"

#include "Graphics/Plot2D/Plotter.h"
#include "Graphics/Plot2D/PlotThemeManager.h"

namespace Tests {
    using namespace Slab;

    VShapeExpansionTest::VShapeExpansionTest()
    : FWindowRow("V-shape expansion test")
    , graph("V-Shape expansion")
    , stats(Graphics::FSlabWindowConfig{"Stats"})
    {
        potential = New<Slab::Math::RtoR::NonlinearKGPotential>(1.0, 15, 1.0);
        derivs = potential->diff(0);

        auto window = New<Graphics::FPlot2DWindow>("V-shape series test");
        auto style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles[0];
        style.filled = false;
        Graphics::Plotter::AddRtoRFunction(window, potential, style, "KG V-shape");
        Graphics::Plotter::AddRtoRFunction(window, derivs, style, "V'");

        AddWindow(Naked(stats), Right, 0.15);
        AddWindow(window);
    }

    void VShapeExpansionTest::ImmediateDraw(const Graphics::FPlatformWindow& PlatformWindow) {
        stats.AddExternalDraw([this](){
            auto A = (float) potential->getA();
            auto s = (float) potential->get_s();
            auto N = (int) potential->getN();

            if (ImGui::SliderFloat("A₀", &A, 0.1, 10)) potential->setA((DevFloat) A);
            if (ImGui::SliderFloat("s", &s, -3, 3)) potential->set_s((DevFloat) s);
            if (ImGui::SliderInt("N", &N, 1, 55, "%d")) {
                if (!(N % 2)) --N;

                potential->setN(N);
            }
        });

        FWindowRow::ImmediateDraw(PlatformWindow);
    }
} // Tests