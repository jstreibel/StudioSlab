//
// Created by joao on 20/09/23.
//

#include "KG-Recipe.h"

#include "Core/Controller/CommandLine/CommandLineInterfaceManager.h"
#include "Math/Numerics/ODE/Steppers/RungeKutta4.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "KG-NumericConfig.h"
#include "Core/SlabCore.h"

namespace Slab::Models {

    KGRecipe::KGRecipe(const TPointer<KGNumericConfig>& numeric_config,
                       const Str &name, const Str& generalDescription, bool doRegister)
            : NumericalRecipe(numeric_config, name, generalDescription, DONT_REGISTER)
            , kg_numeric_config(numeric_config)
            , device_config(DONT_REGISTER){

        const auto default_theme = Slab::Graphics::PlotThemeManager::GetDefault();
        auto themes = Slab::Graphics::PlotThemeManager::GetThemes();
        Str available_themes = plotTheme.getDescription() + " Available themes are: ";
        for(auto &theme : themes)
            available_themes += Str("'") + theme + "', ";

        plotTheme.setDescription(available_themes);
        plotTheme.SetValue(default_theme);

        Interface->AddParameters({&plotTheme,
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

        Interface->AddSubInterface(device_config.GetInterface());

        if (doRegister) {
            RegisterToManager();
        }
    }

    auto KGRecipe::buildStepper() -> TPointer<Stepper> {
        auto solver = buildSolver();

        return New <RungeKutta4> (solver, kg_numeric_config->Getdt());
    }


    void KGRecipe::notifyAllCLArgsSetupFinished() {
        auto nThreads = device_config.get_nThreads();
        auto N = kg_numeric_config->getN(); // This is good for 2D too because it is computed in stripes.
        if (N % nThreads != 0)
            throw Exception("Bad assertion N%nThreads. Expected 0 got "
                            + ToStr(N % nThreads) + ".");

        if(*VisualMonitor) {
            Core::StartBackend("GLFW");
            Graphics::PlotThemeManager::SetTheme(*plotTheme);
        }
        else Core::BackendManager::Startup("Headless");


        FCommandLineInterfaceListener::notifyAllCLArgsSetupFinished();
    }

    void KGRecipe::setupForCurrentThread() {
        NumericalRecipe::setupForCurrentThread();

        device_config.setupForThread();
    }


}