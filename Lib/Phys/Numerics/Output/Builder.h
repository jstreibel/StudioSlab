//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_OUTPUTSTRUCTUREBUILDERBASE_H
#define FIELDS_OUTPUTSTRUCTUREBUILDERBASE_H

#include "Base/Controller/Interface/InterfaceOwner.h"
#include "Base/Controller/Interface/CommonParameters.h"

class OutputManager;


namespace Numerics {

    namespace OutputSystem {

        class Builder : public InterfaceOwner {
        protected:
            BoolParameter::Ptr takeSnapshot                         = BoolParameter::New(false, "s,snapshot", "Take a snapshot of simulation at the end.");
            RealParameter::Ptr snapshotTime                         = RealParameter::New(-1.0, "ss,snapshotTime",
                                          "Force snapshot to be taken at some time prior to end (after will result in no output.");

            BoolParameter::Ptr noHistoryToFile                      = BoolParameter::New(false, "o,no_history_to_file", "Don't output history to file.");
            IntegerParameter::Ptr outputResolution                  = IntegerParameter::New(512, "outN",
                                                 "Output resolution of space dimension in history output.");
            BoolParameter::Ptr VisualMonitor                        = BoolParameter::New(false, "g,visual_monitor", "Monitor simulation visually.");
            BoolParameter::Ptr VisualMonitor_startPaused            = BoolParameter::New(false, "p,visual_monitor_paused", "Start visual monitored "
                                                                             "simulation paused.");
            IntegerParameter::Ptr OpenGLMonitor_stepsPerIdleCall    = IntegerParameter::New(1, "s,steps_per_idle_call",
                                                               "Simulation steps between visual monitor "
                                                               "updates call.");

            explicit Builder(Str generalDescription = "Output System Builder,the builder that plugs output channels (Plugs) to the output socket of the Integrator Program");

            // void buildCommon(OutputManager &outputManager);
            static void addConsoleMonitor(OutputManager &outputManager, int nSteps);

        public:
            typedef std::shared_ptr<Builder> Ptr;

            virtual auto build(Str outputFileName) -> OutputManager * = 0;
        };

    }

}


#endif //FIELDS_OUTPUTSTRUCTUREBUILDERBASE_H
