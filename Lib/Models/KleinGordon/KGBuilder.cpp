//
// Created by joao on 20/09/23.
//

#include "KGBuilder.h"

#include "Core/Controller/Interface/InterfaceManager.h"
#include "Math/Numerics/Method/RungeKutta4.h"
#include "Graphics/Plot2D/PlotThemeManager.h"

namespace Slab::Models {

    KGBuilder::KGBuilder(const Str &name, Str generalDescription, bool doRegister)
            : NumericalRecipe(name, std::move(generalDescription), DONT_REGISTER) {

        auto default_theme = Slab::Graphics::PlotThemeManager::GetDefault();
        auto themes = Slab::Graphics::PlotThemeManager::GetThemes();
        Str available_themes = plotTheme.getDescription() + " Available themes are: ";
        for(auto &theme : themes)
            available_themes += Str("'") + theme + "', ";

        plotTheme.setDescription(available_themes);
        plotTheme.setValue(default_theme);

        interface->addParameters({&plotTheme,
                                  &noHistoryToFile,
                                  &outputResolution,
                                  &VisualMonitor,
                                  &VisualMonitor_startPaused,
                                  &takeSnapshot,
                                  &takeSpaceDFTSnapshot,
                                  &takeTimeDFTSnapshot,
                                  // &timeDFTSnapshot_tStart,
                                  &timeDFTSnapshot_tLength,
                                  &timeDFTSnapshot_tDelta,
                                  // &snapshotTime,
                                  });

        if (doRegister) InterfaceManager::getInstance().registerInterface(interface);
    }

    auto KGBuilder::buildStepper() -> Pointer<Stepper> {
        auto solver = buildEquationSolver();

        return New <RungeKutta4> (solver);
    }


    void KGBuilder::notifyAllCLArgsSetupFinished() {
        auto nThreads = NumericalRecipe::simulationConfig.dev.get_nThreads();
        auto N = NumericalRecipe::simulationConfig.numericConfig.getN();
        if (N % nThreads != 0)
            throw Exception("Bad assertion N%nThreads. Expected 0 got "
                            + ToStr(N % nThreads) + ".");

        if(*VisualMonitor) {
            Graphics::PlotThemeManager::SetTheme(*plotTheme);
        }


        InterfaceListener::notifyAllCLArgsSetupFinished();
    }


}