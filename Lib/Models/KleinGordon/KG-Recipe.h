//
// Created by joao on 8/2/23.
//

#ifndef STUDIOSLAB_KG_RECIPE_H
#define STUDIOSLAB_KG_RECIPE_H

#include <utility>

#include "Math/Numerics/NumericalRecipe.h"
#include "Math/Numerics/ODE/SimConfig/DeviceConfig.h"
#include "KG-NumericConfig.h"

#define DONT_REGISTER false // don touch

namespace Slab::Models {

    using namespace Slab::Core;
    using namespace Slab::Math;

    class FKGOutputOptions final : public FInterfaceOwner
    {
    public:
        StringParameter  PlotTheme                    = StringParameter("Dark", FParameterDescription{"plot_theme", "Choose plotting theme."});
        BoolParameter    TakeSnapshot                   = BoolParameter(false,  FParameterDescription{'s', "snapshot", "Take a snapshot of simulation at the end."});
        RealParameter    snapshotTime                   = RealParameter(-1.0,   FParameterDescription{"snapshotTime", "Force snapshot to be taken at some time prior to end (after will result in no output."});
        BoolParameter    TakeSpaceDFTSnapshot           = BoolParameter(false,  FParameterDescription{"dft_snapshot", "Take a snapshot of discrete Fourier transform (DFT) of field at the end."});
        BoolParameter    TakeTimeDFTSnapshot            = BoolParameter(false,  FParameterDescription{"Time DFT Snapshot", "Take time-domain dft's of field from '--time_dft_tstart' until that plus 'time_dft_length', and repeat every 'time_dft_delta'.", FLongOptionFormatting{true, '-'}});
        /* RealParameter    timeDFTSnapshot_tStart         = RealParameter(0.0 ,FParameterDescription{ "time_dft_start", "Time domain dft starting time."}); */
        RealParameter    TimeDFTSnapshot_tLength        = RealParameter(-1.,    FParameterDescription{"time_dft_length", "Length of time-domain dft snapshots. If left negative, it is all from t=0."});
        RealParameter    TimeDFTSnapshot_tDelta         = RealParameter(-1.,    FParameterDescription{"time_dft_delta", "Interval between each '--time_dft_snapshot'. Leave negative for one single dft snapshot at the end of the simulation."});
        BoolParameter    NoHistoryToFile                = BoolParameter(false,  FParameterDescription{'o', "no_history_to_file", "Don't output history to file."});
        IntegerParameter OutputResolution               = IntegerParameter(512, FParameterDescription{"outN", "Output resolution of space dimension in history output."});
        BoolParameter    VisualMonitor                  = BoolParameter(false,  FParameterDescription{'g', "visual_monitor", "Monitor simulation visually."});
        BoolParameter    VisualMonitor_startPaused      = BoolParameter(false,  FParameterDescription{'p', "visual_monitor_paused", "Start visual monitored simulation paused."});

        explicit FKGOutputOptions(bool bDoRegister);

        [[nodiscard]] Int GetOutputResolution() const { return *OutputResolution; };

    protected:
        auto NotifyAllInterfacesSetupIsFinished() -> void override;
    };

    class KGRecipe : public Base::FNumericalRecipe {

    public:
        explicit KGRecipe(const TPointer<FKGNumericConfig>& numeric_config, const Str& name="Klein-Gordon",
                          const Str& generalDescription="The Klein-Gordon scalar field equation builder",
                          bool doRegister=false);

        [[nodiscard]] TPointer<const FKGNumericConfig> GetKGNumericConfig() const { return KGNumericConfig;}
        [[nodiscard]] FDeviceConfig GetDeviceConfig() const { return DeviceConfig; }
        [[nodiscard]] FKGOutputOptions GetOutputOptions() const { return OutputOptions; };

        virtual void* getHamiltonian() = 0;

        void setupForCurrentThread() override;

        TPointer<Stepper> buildStepper() override;

        virtual auto buildSolver()  -> TPointer<Base::LinearStepSolver> = 0;

        auto NotifyAllInterfacesSetupIsFinished() -> void override;

    protected:
        TPointer<FKGNumericConfig> KGNumericConfig;
        FDeviceConfig              DeviceConfig;
        FKGOutputOptions           OutputOptions;
    };

}

#endif //STUDIOSLAB_KG_RECIPE_H
