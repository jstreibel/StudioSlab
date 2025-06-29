//
// Created by joao on 24/12/24.
//


#include "CrashPad.h"

#include "Core/SlabCore.h"

#include "Graphics/SlabGraphics.h"
#include "Graphics/Window/SlabWindowManager.h"
#include "Graphics/DataViewers/MainViewer.h"
#include "Graphics/DataViewers/Viewers/FourierViewer.h"
#include "Graphics/Plot2D/PlotThemeManager.h"

#include "Math/Numerics/Metropolis/RtoR/RtoR-Action-Metropolis-Recipe.h"
#include "MathApp.h"
#include "Graphics/Plot2D/Plotter.h"

#include "Utils/RandUtils.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/RtoRPolynomial.h"
#include "Models/KleinGordon/RtoR-Montecarlo/RtoR-Hamiltonian-MetropolisHastings-Recipe.h"

using Themes = Slab::Graphics::PlotThemeManager;

void setup_viewer(Slab::Pointer<Slab::Math::RtoR::NumericFunction_CPU> field) {
    Slab::Core::StartBackend("GLFW");

    auto plot_window = Slab::New<Slab::Graphics::Plot2DWindow>("Plot");
    plot_window->Set_x(1700);
    plot_window->NotifyReshape(1200, 800);
    Slab::Graphics::Plotter::AddRtoRFunction(plot_window, field,      Themes::GetCurrent()->FuncPlotStyles[0], "functional");

    auto wm = Slab::New<Slab::Graphics::SlabWindowManager>();
    wm->AddSlabWindow(plot_window, false);

    Slab::Graphics::GetGraphicsBackend()->GetMainSystemWindow()->addAndOwnEventListener(wm);
}

int run(int argc, const char **argv) {
    constexpr unsigned max_steps = -1;
    using Recipe = Slab::Models::KGRtoR::Metropolis::RtoRHamiltonianMetropolisHastingsRecipe;
    auto mc_recipe = Slab::New<Recipe>(max_steps);
    Slab::Core::RegisterCLInterface(mc_recipe->GetInterface());

    auto prog = Slab::New<Slab::Math::MathApp> (argc, argv, mc_recipe);

    setup_viewer(mc_recipe->getField().ϕ);

    return prog->run();
}

int main(int argc, const char **argv) {
    return Slab::SafetyNet::jump(run, argc, argv);
}