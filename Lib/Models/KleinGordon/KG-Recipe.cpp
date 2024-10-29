//
// Created by joao on 20/09/23.
//

#include "KG-Recipe.h"

#include "Core/Controller/CommandLine/CLInterfaceManager.h"
#include "Math/Numerics/ODE/Steppers/RungeKutta4.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "KG-NumericConfig.h"

namespace Slab::Models {

    KGRecipe::KGRecipe(const Pointer<KGNumericConfig>& numeric_config,
                       const Str &name, const Str& generalDescription, bool doRegister)
            : NumericalRecipe(numeric_config, name, generalDescription, DONT_REGISTER)
            , kg_numeric_config(numeric_config)
            , device_config(DONT_REGISTER){

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

        interface->addSubInterface(device_config.getInterface());

        if (doRegister) {
            registerToManager();
        }
    }

    auto KGRecipe::buildStepper() -> Pointer<Stepper> {
        auto solver = buildSolver();

        return New <RungeKutta4> (solver, kg_numeric_config->getdt());
    }


    void KGRecipe::notifyAllCLArgsSetupFinished() {
        auto nThreads = device_config.get_nThreads();
        auto N = kg_numeric_config->getN(); // This is good for 2D too because it is computed in stripes.
        if (N % nThreads != 0)
            throw Exception("Bad assertion N%nThreads. Expected 0 got "
                            + ToStr(N % nThreads) + ".");

        if(*VisualMonitor) {
            Graphics::PlotThemeManager::SetTheme(*plotTheme);
        }


        CLInterfaceListener::notifyAllCLArgsSetupFinished();
    }


}