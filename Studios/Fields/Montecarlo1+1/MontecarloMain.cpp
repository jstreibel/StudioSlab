//
// Created by joao on 3/12/23.
//

#include "CrashPad.h"

#include "Core/SlabCore.h"

#include "Graphics/SlabGraphics.h"
#include "Graphics/Window/SlabWindowManager.h"
#include "Graphics/DataViewers/MainViewer.h"
#include "Graphics/DataViewers/Viewers/FourierViewer.h"

#include "Math/Function/R2toR/Model/FunctionsCollection/AnalyticOscillon_1plus1d.h"

#include "Math/Numerics/Metropolis/R2toR/R2toR-Metropolis-Recipe.h"
#include "MathApp.h"
#include "Graphics/Plot2D/Plotter.h"

void setup_viewer(Slab::Pointer<Slab::Math::R2toR::NumericFunction_CPU> field) {
    Slab::Core::StartBackend("GLFW");

    {
        using Oscillon = Slab::Math::R2toR::AnalyticOscillon_1plus1d;
        field->Set(Oscillon({-.5, 1, 0, 0, 0.64234}));

        fix N = field->getN();
        fix M = field->getM();

        fix cutoff = 3;

        for(int i=cutoff; i<N-cutoff; ++i) {
            for(int j=cutoff; j<M-cutoff; ++j)
                field->At(i, j) = 0;
        }
    }

    auto plot_window = Slab::New<Slab::Graphics::Plot2DWindow>();
    auto arts = Slab::Graphics::Plotter::AddR2toRFunction(plot_window, field, "ϕ(t,x)");
    arts->setDataMutable(true);

    // auto viewer = Slab::New<Slab::Graphics::MainViewer>();
    // viewer->addViewer(Slab::New<Slab::Graphics::FourierViewer>(viewer->getGUIWindow()));
    // viewer->setFunction(field);

    auto wm = Slab::New<Slab::Graphics::SlabWindowManager>();
    wm->addSlabWindow(plot_window);

    Slab::Graphics::GetGraphicsBackend().addAndOwnEventListener(wm);
}

int run(int argc, const char **argv) {
    constexpr auto max_steps = 1000000;
    auto mc_recipe = Slab::New<Slab::Math::R2toRMetropolisRecipe>(max_steps);
    Slab::Core::RegisterCLInterface(mc_recipe->getInterface());

    auto prog = Slab::New<Slab::Math::MathApp> (argc, argv, mc_recipe);

    setup_viewer(mc_recipe->getField());


    return prog->run();
}

int main(int argc, const char **argv) {
    return Slab::Core::SafetyNet::jump(run, argc, argv);
}