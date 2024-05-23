//
// Created by joao on 7/20/23.
//

#ifndef STUDIOSLAB_NUMERIC_BUILDER_H
#define STUDIOSLAB_NUMERIC_BUILDER_H

#include "Math/Numerics/SimConfig/DeviceConfig.h"
#include "Math/Numerics/SimConfig/NumericConfig.h"
#include "Math/Numerics/Method/Stepper.h"
#include "Math/Numerics/Output/OutputManager.h"

#include "Core/Controller/Interface/Interface.h"
#include "Core/Controller/Interface/InterfaceOwner.h"
#include "Math/DifferentialEquations/EquationSolver.h"
#include "Math/DifferentialEquations/BoundaryConditions.h"
#include "Math/Numerics/Method/Method-RK4.h"

#define BUILDER_IMPL

namespace Slab::Math {

    template<typename EquationSolverType>
    class Builder : public InterfaceOwner {
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
        IntegerParameter OpenGLMonitor_stepsPerIdleCall = IntegerParameter(1, "steps_per_idle_call",
                                                                           "Simulation steps between visual monitor updates call.");

        Str prefix = "";

        explicit Builder(Str name, Str generalDescription);

        NumericConfig numericParams;
        DeviceConfig dev;

    public:
        typedef std::shared_ptr<Builder> Ptr;

        using EqSolver          = EquationSolverType;
        using EqState           = EqSolver::EqState;
        using BoundaryCondition = Base::BoundaryConditions<EqState>;

        virtual ~Builder() {}

        virtual auto buildOutputManager()         -> OutputManager * = 0;

        virtual auto getBoundary()                -> BoundaryCondition * = 0;
        virtual auto getInitialState()            -> EqState       * = 0;
        virtual auto getEquationSolver()          -> EqSolver      * = 0;

        virtual auto getMethod()                  -> Stepper*;

        auto getNumericParams()             const -> const NumericConfig &;
        auto getDevice()                    const -> const DeviceConfig &;
        auto toString()                     const -> Str;
    };


} // Simulation


#ifdef BUILDER_IMPL

#include "Core/Controller/Interface/InterfaceManager.h"
#include "Core/Tools/Log.h"
#include "Math/Numerics/Method/Method-RK4.h"

#define DONT_REGISTER false

namespace Slab::Math {

    template<typename SolverType>
    Builder<SolverType>::Builder(Str name, Str generalDescription)
            : InterfaceOwner(name, 100, DONT_REGISTER)
            , numericParams(DONT_REGISTER)
            , dev(DONT_REGISTER)
            , prefix(name)
    {
        interface->addParameters({&noHistoryToFile, &outputResolution,
                                  &VisualMonitor, &VisualMonitor_startPaused, &OpenGLMonitor_stepsPerIdleCall
                                         /*&takeSnapshot, &snapshotTime, */ });

        interface->addSubInterface(numericParams.getInterface());
        interface->addSubInterface(dev.getInterface());

        Log::Info() << "SimulationBuilder '" << interface->getName() << "': \""
                    << interface->getGeneralDescription() << "\" instantiated." << Log::Flush;
    }

    template<typename SolverType>
    auto Builder<SolverType>::getNumericParams() const -> const NumericConfig & {
        return numericParams;
    }

    template<typename SolverType>
    auto Builder<SolverType>::getDevice() const -> const DeviceConfig & {
        return dev;
    }

    template<typename SolverType>
    auto Builder<SolverType>::toString() const -> Str {
        auto strParams = interface->toString();

        auto str = prefix + "-" + strParams;

        return str;
    }

    template<typename SolverType>
    auto Builder<SolverType>::getMethod() -> Stepper * {
        auto &u_0 = *getInitialState();
        auto &solver = *getEquationSolver();

        return new StepperRK4<typename SolverType::EqState>(solver, u_0);
    }
}

#endif

#endif //STUDIOSLAB_BUILDER_H
