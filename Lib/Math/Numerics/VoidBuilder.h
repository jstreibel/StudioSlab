#ifndef SIMULATION_H
#define SIMULATION_H

#include "Math/Numerics/Method/Stepper.h"
#include "Math/Numerics/Output/OutputManager.h"

#include "Core/Controller/Interface/Interface.h"
#include "Math/Numerics/SimConfig/SimulationConfig.h"

namespace Core::Simulation {

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
        IntegerParameter OpenGLMonitor_stepsPerIdleCall = IntegerParameter(-1, "s,steps_per_idle_call",
                                                                              "Simulation steps between visual monitor updates call. Leave negative for auto.");

        SimulationConfig simulationConfig;

        Str prefix;

        explicit VoidBuilder(Str name, Str generalDescription, bool doRegister = false);

    public:
        typedef std::shared_ptr<VoidBuilder> Ptr;

        VoidBuilder() = delete;
        virtual ~VoidBuilder() = default;

        virtual auto buildOutputManager()         -> OutputManager * = 0;
        virtual auto buildEquationSolver()        -> void * = 0;
        virtual auto buildStepper()               -> Stepper* = 0;

        virtual auto suggestFileName()      const -> Str;

        virtual auto getBoundary()                -> void * = 0;
        virtual auto getInitialState()            -> void * = 0;


        virtual
        auto newFunctionArbitrary()       -> void * = 0;
        template<class ARB_FUNC_TYPE>
        auto NewFunctionArbitrary()       -> ARB_FUNC_TYPE *;

        virtual
        auto newFieldState()              -> void * = 0;
        template<typename StateType>
        auto NewFieldState()              -> StateType*;

        auto getNumericParams()             const -> const NumericConfig &;
        auto getDevice()                    const -> const DeviceConfig &;
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



#endif // SIMULATION_H
