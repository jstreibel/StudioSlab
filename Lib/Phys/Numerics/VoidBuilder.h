#ifndef SIMULATION_H
#define SIMULATION_H

#include "Phys/Toolset/Device.h"
#include "Phys/Numerics/Program/NumericParams.h"
#include "Phys/Numerics/Method/Method.h"
#include "Phys/Numerics/Output/OutputManager.h"

#include "Base/Controller/Interface/Interface.h"

namespace Base {

    namespace Simulation {

        class VoidBuilder : public InterfaceOwner {
        protected:
            BoolParameter    takeSnapshot                   = BoolParameter(false, "s,snapshot", "Take a snapshot of simulation at the end.");

            RealParameter    snapshotTime                   = RealParameter(-1.0, "ss,snapshotTime",
                                                                               "Force snapshot to be taken at some time prior to end (after will result in no output.");
            BoolParameter    noHistoryToFile                = BoolParameter(false, "o,no_history_to_file", "Don't output history to file.");

            IntegerParameter outputResolution               = IntegerParameter(512, "outN",
                                                                                  "Output resolution of space dimension in history output.");
            BoolParameter    VisualMonitor                  = BoolParameter(false, "g,visual_monitor", "Monitor simulation visually.");

            BoolParameter    VisualMonitor_startPaused      = BoolParameter(false, "p,visual_monitor_paused", "Start visual monitored "
                                                                                                                 "simulation paused.");
            IntegerParameter OpenGLMonitor_stepsPerIdleCall = IntegerParameter(1, "s,steps_per_idle_call",
                                                                                  "Simulation steps between visual monitor updates call.");

            Str prefix = "";

            explicit VoidBuilder(Str name, Str generalDescription);

            NumericParams numericParams;
            Device dev;

        public:
            virtual ~VoidBuilder() {}

            typedef std::shared_ptr<VoidBuilder> Ptr;

            virtual auto getBoundary()                -> void *    = 0;
            virtual auto buildOutputManager()         -> OutputManager * = 0;

            virtual auto newFunctionArbitrary()       -> void * = 0;
            virtual auto newFieldState()              -> void * = 0;
            virtual auto getSystemSolver()            -> void * = 0;

            virtual auto buildStepper() -> Method*;

            template<typename StateType>  auto NewFieldState()        -> StateType*;
            template<class ARB_FUNC_TYPE> auto NewFunctionArbitrary() -> ARB_FUNC_TYPE *;

            auto getNumericParams()             const -> const NumericParams &;
            auto getDevice()                    const -> const Device &;
            auto toString()                     const -> Str;
        };

        template<typename StateType>
        auto VoidBuilder::NewFieldState() -> StateType * {
            return (StateType*)this->newFieldState();
        }

        template<class ARB_FUNC_TYPE>
        ARB_FUNC_TYPE *VoidBuilder::NewFunctionArbitrary() {
            return (ARB_FUNC_TYPE *) newFunctionArbitrary();
        }
    }
}


#endif // SIMULATION_H
