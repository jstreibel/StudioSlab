//
// Created by joao on 20/09/23.
//

#include "KG-Recipe.h"

#include "../../Core/Controller/CommandLineInterfaceManager.h"
#include "Math/Numerics/ODE/Steppers/RungeKutta4.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "KG-NumericConfig.h"
#include "Core/SlabCore.h"

namespace Slab::Models {

    KGRecipe::KGRecipe(const TPointer<FKGNumericConfig>& numeric_config,
                       const Str &name, const Str& generalDescription, bool doRegister)
            : FNumericalRecipe(numeric_config, name, generalDescription, DONT_REGISTER)
            , KGNumericConfig(numeric_config)
            , DeviceConfig(DONT_REGISTER){

        const auto default_theme = Slab::Graphics::PlotThemeManager::GetDefault();
        auto themes = Slab::Graphics::PlotThemeManager::GetThemes();
        Str available_themes = plotTheme.getDescription() + " Available themes are: ";
        for(auto &theme : themes)
            available_themes += Str("'") + theme + "', ";

        plotTheme.setDescription(available_themes);
        plotTheme.SetValue(default_theme);

        Interface->AddParameters({&plotTheme,
                                  &NoHistoryToFile,
                                  &OutputResolution,
                                  &VisualMonitor,
                                  &VisualMonitor_startPaused,
                                  &TakeSnapshot,
                                  &TakeSpaceDFTSnapshot,
                                  &TakeTimeDFTSnapshot,
                                  // &timeDFTSnapshot_tStart,
                                  &TimeDFTSnapshot_tLength,
                                  &TimeDFTSnapshot_tDelta,
                                  // &snapshotTime,
                                  });

        Interface->AddSubInterface(DeviceConfig.GetInterface());

        if (doRegister) {
            RegisterToManager();
        }
    }

    auto KGRecipe::buildStepper() -> TPointer<Stepper> {
        auto solver = buildSolver();

        return New <RungeKutta4> (solver, KGNumericConfig->Getdt());
    }


    void KGRecipe::NotifyAllCLArgsSetupFinished() {
        auto nThreads = DeviceConfig.get_nThreads();
        auto N = KGNumericConfig->getN(); // This is good for 2D too because it is computed in stripes.
        if (N % nThreads != 0)
            throw Exception("Bad assertion N%nThreads. Expected 0 got "
                            + ToStr(N % nThreads) + ".");

        if(*VisualMonitor) {
            Core::StartBackend("GLFW");
            Graphics::PlotThemeManager::SetTheme(*plotTheme);
        }
        else Core::BackendManager::Startup("Headless");


        FNumericalRecipe::NotifyAllCLArgsSetupFinished();
    }

    void KGRecipe::setupForCurrentThread() {
        FNumericalRecipe::setupForCurrentThread();

        DeviceConfig.setupForThread();
    }


}