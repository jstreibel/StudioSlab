//
// Created by joao on 3/12/23.
//

#include "CrashPad.h"

#include "Core/SlabCore.h"

#include "Graphics/SlabGraphics.h"
#include "Graphics/Window/SlabWindowManager.h"
#include "Graphics/DataViewers/MainViewer.h"
#include "Graphics/DataViewers/Viewers/FourierViewer.h"
#include "Graphics/Plot2D/PlotThemeManager.h"

#include "Math/Numerics/Metropolis/RtoR/RtoR-Metropolis-Recipe.h"
#include "MathApp.h"
#include "Graphics/Plot2D/Plotter.h"

#include "Utils/RandUtils.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/RtoRPolynomial.h"

using Themes = Slab::Graphics::PlotThemeManager;

void setup_viewer(Slab::Pointer<Slab::Math::RtoR::NumericFunction_CPU> field) {
    Slab::Core::StartBackend("GLFW");

    Slab::Pointer<Slab::Math::RtoR::Function> trajectory;
    {
        fix N = field->N;
        OUT data = field->getSpace().getHostData();
        for(int n=0; n<N; ++n) data[n] = Slab::RandUtils::RandomUniformReal(-0.1, 2);

        fix q_0 = 0;
        fix q_f = 1;

        data[0]   = q_0;
        data[N-1] = q_f;

        fix t_0 = field->xMin;
        fix t_f = field->xMax;

        fix v_avg = (q_f-q_0)/(t_f-t_0);

        using namespace Slab;
        using Poly = Math::RtoR::RtoRPolynomial;
        Vector<Real> coeffs = {(q_0-v_avg*t_0-t_0*t_f), (v_avg + .5*(t_f+t_0)), -1/2.};
        trajectory = New<Poly>(coeffs);
    }

    auto plot_window = Slab::New<Slab::Graphics::Plot2DWindow>("Plot");
    plot_window->setx(1700);
    plot_window->notifyReshape(1200, 800);
    Slab::Graphics::Plotter::AddRtoRFunction(plot_window, field,      Themes::GetCurrent()->funcPlotStyles[0], "functional");
    auto ground_style = Themes::GetCurrent()->funcPlotStyles[1].permuteColors(true);
    ground_style.filled = false;
    Slab::Graphics::Plotter::AddRtoRFunction(plot_window, trajectory, ground_style, "ground truth");

    auto wm = Slab::New<Slab::Graphics::SlabWindowManager>();
    wm->addSlabWindow(plot_window);

    Slab::Graphics::GetGraphicsBackend()->GetMainSystemWindow()->addAndOwnEventListener(wm);
}

int run(int argc, const char **argv) {
    constexpr unsigned max_steps = -1;
    auto mc_recipe = Slab::New<Slab::Math::RtoRMetropolisRecipe>(max_steps);
    Slab::Core::RegisterCLInterface(mc_recipe->getInterface());

    auto prog = Slab::New<Slab::Math::MathApp> (argc, argv, mc_recipe);

    setup_viewer(mc_recipe->getField());


    return prog->run();
}

int main(int argc, const char **argv) {
    return Slab::SafetyNet::jump(run, argc, argv);
}