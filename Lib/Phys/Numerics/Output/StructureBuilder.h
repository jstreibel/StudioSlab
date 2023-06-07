//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_OUTPUTSTRUCTUREBUILDERBASE_H
#define FIELDS_OUTPUTSTRUCTUREBUILDERBASE_H

#include "Base/Controller/Interface/Interface.h"
#include "Base/Controller/Interface/CommonParameters.h"

class OutputManager;


namespace Numerics {

    namespace OutputSystem {

        class StructureBuilder : public Interface {
            BoolParameter takeSnapshot = {false, "snapshot,s", "Take a snapshot of simulation at the end."};
            RealParameter snapshotTime = {-1.0, "snapshotTime,t",
                                          "Force snapshot to be taken at some time prior to end (after will result in no output."};

        protected:
            BoolParameter noHistoryToFile = {false, "no_history_to_file,o", "Don't output history to file."};
            IntegerParameter outputResolution = {512, "outN",
                                                 "Output resolution of space dimension in history output."};
            BoolParameter OpenGLMonitor = {false, "gl,g", "Monitor simulation using OpenGL output."};
            BoolParameter OpenGLMonitor_startPaused = {false, "gl_paused,p", "Start OpenGL monitor paused."};
            IntegerParameter OpenGLMonitor_stepsPerIdleCall = {1, "steps_per_idle_call,s",
                                                               "Simulatoin steps per GLUT idle call."};

            explicit StructureBuilder(String generalDescription = "Simulation output structure");

            // void buildCommon(OutputManager &outputManager);
            static void addConsoleMonitor(OutputManager &outputManager, int nSteps);

        public:
            virtual auto build(String outputFileName) -> OutputManager * = 0;
        };

    }

}


#endif //FIELDS_OUTPUTSTRUCTUREBUILDERBASE_H
