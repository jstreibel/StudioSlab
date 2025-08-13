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

    class KGRecipe : public Base::NumericalRecipe {
    protected:
        StringParameter    plotTheme                    = StringParameter("Dark", "plot_theme", "Choose plotting theme.");



        BoolParameter    TakeSnapshot                   = BoolParameter(false, "s,snapshot",
                                                                        "Take a snapshot of simulation at the end.");
        RealParameter    snapshotTime                   = RealParameter(-1.0, "ss,snapshotTime",
                                                                        "Force snapshot to be taken at some time prior "
                                                                        "to end (after will result in no output.");



        BoolParameter    TakeSpaceDFTSnapshot           = BoolParameter(false, "dft_snapshot",
                                                                        "Take a snapshot of discrete Fourier transform "
                                                                        "(DFT) of field at the end.");


        BoolParameter    TakeTimeDFTSnapshot            = BoolParameter(false, "time_dft_snapshot",
                                                                        "Take time domain dft's of field from "
                                                                        "'--time_dft_tstart' until that plus "
                                                                        "'time_dft_length', and repeat every "
                                                                        "'time_dft_delta'.");
        /*RealParameter    timeDFTSnapshot_tStart         = RealParameter(0.0 , "time_dft_start",
                                                                        "Time domain dft starting time.");
                                                                        */
        RealParameter    timeDFTSnapshot_tLength        = RealParameter(-1., "time_dft_length",
                                                                        "Length of time-domain dft snapshots. If left "
                                                                        "negative, it is all from t=0.");
        RealParameter    timeDFTSnapshot_tDelta         = RealParameter(-1., "time_dft_delta",
                                                                         "Interval between each '--time_dft_snapshot'. "
                                                                         "Leave negative for one single dft snapshot at "
                                                                         "the end of the simulation.");




        BoolParameter    NoHistoryToFile                = BoolParameter(false, "o,no_history_to_file",
                                                                        "Don't output history to file.");

        IntegerParameter OutputResolution               = IntegerParameter(512, "outN",
                                                                           "Output resolution of space dimension in "
                                                                           "history output.");
        BoolParameter    VisualMonitor                  = BoolParameter(false, "g,visual_monitor",
                                                                        "Monitor simulation visually.");

        BoolParameter    VisualMonitor_startPaused      = BoolParameter(false, "p,visual_monitor_paused",
                                                                        "Start visual monitored simulation paused.");

        TPointer<KGNumericConfig> kg_numeric_config;
        Math::DeviceConfig device_config;
    public:
        explicit KGRecipe(const TPointer<KGNumericConfig>& numeric_config, const Str& name="Klein-Gordon",
                          const Str& generalDescription="The Klein-Gordon scalar field equation builder",
                          bool doRegister=false);

        virtual void* getHamiltonian() = 0;

        void setupForCurrentThread() override;

        TPointer<Stepper> buildStepper() override;

        virtual auto buildSolver()  -> TPointer<Base::LinearStepSolver> = 0;

        auto notifyAllCLArgsSetupFinished() -> void override;
    };

}

#endif //STUDIOSLAB_KG_RECIPE_H
