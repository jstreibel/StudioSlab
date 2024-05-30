//
// Created by joao on 8/2/23.
//

#ifndef STUDIOSLAB_KGBUILDER_H
#define STUDIOSLAB_KGBUILDER_H

#include <utility>

#include "Math/Numerics/NumericalRecipe.h"

#define DONT_REGISTER false // don touch

namespace Slab::Models {

    using namespace Slab::Core;
    using namespace Slab::Math;

    class KGBuilder : public Base::NumericalRecipe {
    protected:
        BoolParameter    takeSnapshot                   = BoolParameter(false, "s,snapshot", "Take a snapshot of simulation at the end.");
        BoolParameter    takeDFTSnapshot                = BoolParameter(false,  "dft_snapshot", "Take a snapshot of discrete Fourier transform (DFT) of field at the end.");

        RealParameter    snapshotTime                   = RealParameter(-1.0, "ss,snapshotTime",
                                                                        "Force snapshot to be taken at some time prior to end (after will result in no output.");
        BoolParameter    noHistoryToFile                = BoolParameter(false, "o,no_history_to_file", "Don't output history to file.");

        IntegerParameter outputResolution               = IntegerParameter(512, "outN",
                                                                           "Output resolution of space dimension in history output.");
        BoolParameter    VisualMonitor                  = BoolParameter(false, "g,visual_monitor", "Monitor simulation visually.");

        BoolParameter    VisualMonitor_startPaused      = BoolParameter(false, "p,visual_monitor_paused", "Start visual monitored "
                                                                                                          "simulation paused.");

    public:
        explicit KGBuilder(const Str& name="Klein-Gordon",
                  Str generalDescription="The Klein-Gordon scalar field equation builder",
                  bool doRegister=false);;

        virtual void* getHamiltonian() = 0;

        Stepper* buildStepper() override;

        auto notifyAllCLArgsSetupFinished() -> void override;
    };

}

#endif //STUDIOSLAB_KGBUILDER_H
