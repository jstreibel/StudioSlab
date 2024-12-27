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
#include "Utils/RandUtils.h"

void setup_viewer(const Slab::Pointer<Slab::Math::R2toR::NumericFunction_CPU>& field) {
    Slab::Core::StartBackend("GLFW");

    {
        using Oscillon = Slab::Math::R2toR::AnalyticOscillon_1plus1d;
        fix l = 2.;
        field->Set(Oscillon({-.5*l, l, 0, 0, 0.64234}));

        fix N = field->getN();
        fix M = field->getM();

        fix cutoff_x = 1; //N/4;
        fix cutoff_y = 1; //M/4;

        for(int i=cutoff_x; i<N-cutoff_x; ++i) {
            for(int j=cutoff_y; j<M-cutoff_y; ++j)
                field->At(i, j) = Slab::RandUtils::RandomUniformReal(-.125, .125);
        }
    }

    auto plot_window = Slab::New<Slab::Graphics::Plot2DWindow>("Field plot");
    plot_window->setx(1700);
    plot_window->notifyReshape(1200, 800);
    fix lims = field->getDomain();
    plot_window->getRegion().setLimits(lims.xMin, lims.xMax, lims.yMin, lims.yMax);
    auto arts = Slab::Graphics::Plotter::AddR2toRFunction(plot_window, field, "ϕ(t,x)");
    arts->setDataMutable(true);

    auto viewer = Slab::New<Slab::Graphics::MainViewer>();
    viewer->addViewer(Slab::New<Slab::Graphics::FourierViewer>(viewer->getGUIWindow()));
    viewer->setFunction(field);

    auto wm = Slab::New<Slab::Graphics::SlabWindowManager>();
    wm->addSlabWindow(plot_window);
    wm->addSlabWindow(viewer);

    auto main_syswin = Slab::Graphics::GetGraphicsBackend()->GetMainSystemWindow();
    main_syswin->addAndOwnEventListener(wm);
}

int run(int argc, const char **argv) {
    constexpr unsigned long max_steps = -1;
    auto mc_recipe = Slab::New<Slab::Math::R2toRMetropolisRecipe>(max_steps);
    Slab::Core::RegisterCLInterface(mc_recipe->getInterface());

    auto prog = Slab::New<Slab::Math::MathApp> (argc, argv, mc_recipe);

    setup_viewer(mc_recipe->getField());


    return prog->run();
}

int main(int argc, const char **argv) {
    return Slab::SafetyNet::jump(run, argc, argv);
}