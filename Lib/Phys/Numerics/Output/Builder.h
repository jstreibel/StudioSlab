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
            BoolParameter takeSnapshot                         = BoolParameter(false, "s,snapshot", "Take a snapshot of simulation at the end.");
            RealParameter snapshotTime                         = RealParameter(-1.0, "ss,snapshotTime",
                                          "Force snapshot to be taken at some time prior to end (after will result in no output.");

            BoolParameter noHistoryToFile                      = BoolParameter(false, "o,no_history_to_file", "Don't output history to file.");
            IntegerParameter outputResolution                  = IntegerParameter(512, "outN",
                                                 "Output resolution of space dimension in history output.");
            BoolParameter VisualMonitor                        = BoolParameter(false, "g,visual_monitor", "Monitor simulation visually.");
            BoolParameter VisualMonitor_startPaused            = BoolParameter(false, "p,visual_monitor_paused", "Start visual monitored "
                                                                             "simulation paused.");
            IntegerParameter OpenGLMonitor_stepsPerIdleCall    = IntegerParameter(1, "s,steps_per_idle_call",
                                                               "Simulation steps between visual monitor updates call.");

            explicit Builder(Str name, Str descr);

            // void buildCommon(OutputManager &outputManager);
            static void addConsoleMonitor(OutputManager &outputManager, int nSteps);

        public:
            typedef std::shared_ptr<Builder> Ptr;

            virtual auto build(Str outputFileName) -> OutputManager * = 0;
        };

    }

}


#endif //FIELDS_OUTPUTSTRUCTUREBUILDERBASE_H
