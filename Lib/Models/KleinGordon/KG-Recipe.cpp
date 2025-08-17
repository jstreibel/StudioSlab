//
// Created by joao on 20/09/23.
//

#include "KG-Recipe.h"

#include "../../Core/Controller/InterfaceManager.h"
#include "Math/Numerics/ODE/Steppers/RungeKutta4.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "KG-NumericConfig.h"
#include "Core/SlabCore.h"

namespace Slab::Models {
    FKGOutputOptions::FKGOutputOptions(bool bDoRegister)
    : FInterfaceOwner("Output Options", 0, DONT_REGISTER)
    {
        const auto DefaultTheme = Graphics::PlotThemeManager::GetDefault();
        auto Themes = Graphics::PlotThemeManager::GetThemes();
        Str AvailableThemes = PlotTheme.GetDescription() + " Available themes are: ";
        for(auto &theme : Themes)
            AvailableThemes += Str("'") + theme + "', ";

        PlotTheme.SetDescription(AvailableThemes);
        PlotTheme.SetValue(DefaultTheme);

        Interface->AddParameters({&PlotTheme,
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

        if (bDoRegister) RegisterToManager();
    }

    void FKGOutputOptions::NotifyAllCLArgsSetupFinished()
    {
        FInterfaceOwner::NotifyAllCLArgsSetupFinished();

        if(*VisualMonitor) {
            StartBackend("GLFW");
            Graphics::PlotThemeManager::SetTheme(*PlotTheme);
        }
        else BackendManager::Startup("Headless");
    }


    KGRecipe::KGRecipe(const TPointer<FKGNumericConfig>& numeric_config,
                       const Str &name, const Str& generalDescription, bool doRegister)
            : FNumericalRecipe(numeric_config, name, generalDescription, DONT_REGISTER)
            , KGNumericConfig(numeric_config)
            , DeviceConfig(DONT_REGISTER)
            , OutputOptions(DONT_REGISTER)
    {
        Interface->AddSubInterface(DeviceConfig.GetInterface());
        Interface->AddSubInterface(OutputOptions.GetInterface());

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

        FNumericalRecipe::NotifyAllCLArgsSetupFinished();
    }

    void KGRecipe::setupForCurrentThread() {
        FNumericalRecipe::setupForCurrentThread();

        DeviceConfig.setupForThread();
    }


}