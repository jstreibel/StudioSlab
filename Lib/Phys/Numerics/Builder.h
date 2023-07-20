//
// Created by joao on 7/20/23.
//

#ifndef STUDIOSLAB_BUILDER_H
#define STUDIOSLAB_BUILDER_H

#include "Phys/Toolset/Device.h"
#include "Phys/Numerics/Program/NumericParams.h"
#include "Phys/Numerics/Method/Method.h"
#include "Phys/Numerics/Output/OutputManager.h"

#include "Base/Controller/Interface/Interface.h"
#include "Base/Controller/Interface/InterfaceOwner.h"
#include "Phys/DifferentialEquations/DifferentialEquation.h"

#define BUILDER_IMPL

#define BuilderMap(Mapping) Base::Simulation::Builder<Mapping::BoundaryCondition, Mapping::DiscreteFunction, Mapping::EquationState>

namespace Base::Simulation {

    template<typename BCType, typename DiscrFuncType, typename EqStateType>
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
        IntegerParameter OpenGLMonitor_stepsPerIdleCall = IntegerParameter(1, "s,steps_per_idle_call",
                                                                           "Simulation steps between visual monitor updates call.");

        Str prefix = "";

        explicit Builder(Str name, Str generalDescription);

        NumericParams numericParams;
        Device dev;

    public:
        virtual ~Builder() {}

        typedef std::shared_ptr<Builder> Ptr;

        virtual auto getBoundary()                -> BCType * = 0;
        virtual auto buildOutputManager()         -> OutputManager              * = 0;

        virtual auto newFunctionArbitrary()       -> DiscrFuncType  * = 0;
        virtual auto newFieldState()              -> EqStateType     * = 0;
        virtual auto getEquationSolver()          -> Base::DifferentialEquation<EqStateType> * = 0;

        virtual auto getMethod()                  -> Method*;

        auto getNumericParams()             const -> const NumericParams &;
        auto getDevice()                    const -> const Device &;
        auto toString()                     const -> Str;
    };


} // Simulation


#ifdef BUILDER_IMPL

#include "Base/Controller/Interface/InterfaceManager.h"
#include "Common/Log/Log.h"
#include "Phys/Numerics/Method/Method-RK4.h"

#define GENERATE_FOR_NTHREADS(STEPPER_TYPE, N) \
case (N): \
stepper = new STEPPER_TYPE<N, EqStateType>(dPhi); \
break;

#define DONT_REGISTER false

namespace Base::Simulation {

    template<typename BCType, typename DiscrFuncType, typename EqStateType>
    Builder<BCType, DiscrFuncType, EqStateType>::Builder(Str name, Str generalDescription)
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

    template<typename BCType, typename DiscrFuncType, typename EqStateType>
    auto Builder<BCType, DiscrFuncType, EqStateType>::getNumericParams() const -> const NumericParams & {
        return numericParams;
    }

    template<typename BCType, typename DiscrFuncType, typename EqStateType>
    auto Builder<BCType, DiscrFuncType, EqStateType>::getDevice() const -> const Device & {
        return dev;
    }

    template<typename BCType, typename DiscrFuncType, typename EqStateType>
    auto Builder<BCType, DiscrFuncType, EqStateType>::toString() const -> Str {
        auto strParams = interface->toString();

        auto str = prefix + "-" + strParams;

        return str;
    }

    template<typename BCType, typename DiscrFuncType, typename EqStateType>
    auto Builder<BCType, DiscrFuncType, EqStateType>::getMethod() -> Method * {
        /*
            if(theMethod==RK4){
                switch (numThreads) {
                    GENERATE_FOR_NTHREADS(StepperRK4, 1);
                    GENERATE_FOR_NTHREADS(StepperRK4, 2);
                    GENERATE_FOR_NTHREADS(StepperRK4, 3);
                    GENERATE_FOR_NTHREADS(StepperRK4, 4);
                    GENERATE_FOR_NTHREADS(StepperRK4, 5);
                    GENERATE_FOR_NTHREADS(StepperRK4, 6);
                    GENERATE_FOR_NTHREADS(StepperRK4, 7);
                    GENERATE_FOR_NTHREADS(StepperRK4, 8);
                    GENERATE_FOR_NTHREADS(StepperRK4, 9);
                    GENERATE_FOR_NTHREADS(StepperRK4, 10);
                    GENERATE_FOR_NTHREADS(StepperRK4, 11);
                    GENERATE_FOR_NTHREADS(StepperRK4, 12);
                    GENERATE_FOR_NTHREADS(StepperRK4, 13);
                    GENERATE_FOR_NTHREADS(StepperRK4, 14);
                    GENERATE_FOR_NTHREADS(StepperRK4, 15);
                    GENERATE_FOR_NTHREADS(StepperRK4, 16);
                    default:
                        throw "Number of threads must be between 1 and 16 inclusive.";
                }
            } else if(theMethod == Montecarlo) {
                switch (numThreads) {
                    GENERATE_FOR_NTHREADS(StepperMontecarlo, 1);
                    GENERATE_FOR_NTHREADS(StepperMontecarlo, 2);
                    GENERATE_FOR_NTHREADS(StepperMontecarlo, 3);
                    GENERATE_FOR_NTHREADS(StepperMontecarlo, 4);
                    GENERATE_FOR_NTHREADS(StepperMontecarlo, 5);
                    GENERATE_FOR_NTHREADS(StepperMontecarlo, 6);
                    GENERATE_FOR_NTHREADS(StepperMontecarlo, 7);
                    GENERATE_FOR_NTHREADS(StepperMontecarlo, 8);
                    GENERATE_FOR_NTHREADS(StepperMontecarlo, 9);
                    GENERATE_FOR_NTHREADS(StepperMontecarlo, 10);
                    GENERATE_FOR_NTHREADS(StepperMontecarlo, 11);
                    GENERATE_FOR_NTHREADS(StepperMontecarlo, 12);
                    GENERATE_FOR_NTHREADS(StepperMontecarlo, 13);
                    GENERATE_FOR_NTHREADS(StepperMontecarlo, 14);
                    GENERATE_FOR_NTHREADS(StepperMontecarlo, 15);
                    GENERATE_FOR_NTHREADS(StepperMontecarlo, 16);
                    default:
                        throw "Number of threads must be between 1 and 16 inclusive.";
                }
            } else throw "Unknown integration method.";
             */

        throw "Bad builder";
    }
}

#endif

#endif //STUDIOSLAB_BUILDER_H
