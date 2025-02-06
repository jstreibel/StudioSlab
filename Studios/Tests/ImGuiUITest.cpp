//
// Created by joao on 05/02/25.
//

#include "ImGuiUITest.h"

#include "Graphics/ImGui/ImGuiWindow.h"
#include "Graphics/Plot2D/Plotter.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/RtoRPolynomial.h"

namespace Tests {
    Slab::Pointer<Slab::Graphics::SlabWindow> GetImGuiTestWindow() {
        using Plotter = Slab::Graphics::Plotter;
        auto theme = Slab::Graphics::PlotThemeManager::GetCurrent();
        auto plot_window = Slab::New<Slab::Graphics::Plot2DWindow>("Test Window");

        auto funky = Slab::New<Slab::Math::RtoR::RtoRPolynomial>(Slab::RealVector{0.25, 1.25});
        Plotter::AddRtoRFunction(plot_window, funky, theme->funcPlotStyles[0], "A poly");

        return Slab::New<Slab::Graphics::ImGuiWindow>(plot_window);
    }
} // Tests