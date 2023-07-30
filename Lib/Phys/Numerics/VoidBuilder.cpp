//
// Created by joao on 10/17/21.
//

#include "Base/Controller/Interface/InterfaceSelector.h"
#include "Base/Controller/Interface/InterfaceManager.h"
#include "Common/Log/Log.h"
#include "VoidBuilder.h"
//#include "Phys/Numerics/Method/Method-RK4.h"

#define GENERATE_FOR_NTHREADS(STEPPER_TYPE, N) \
case (N): \
stepper = new STEPPER_TYPE<N, EqStateType>(dPhi); \
break;

#define DONT_REGISTER false

Base::Simulation::VoidBuilder::VoidBuilder(Str name, Str generalDescription)
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

auto Base::Simulation::VoidBuilder::getNumericParams() const -> const NumericParams & {
    return numericParams;
}

auto Base::Simulation::VoidBuilder::getDevice() const -> const Device & {
    return dev;
}

Str Base::Simulation::VoidBuilder::buildFileName() const {
    const auto SEPARATOR = " ";
    auto strParams = numericParams.getInterface()->toString({"L", "N"}, SEPARATOR);

    auto str = prefix + SEPARATOR + strParams;

    return str;
}

/*
auto Base::Simulation::VoidBuilder::buildStepper() -> Method * {
        // if(theMethod==RK4)
        {
            auto &u_0 = *getInitialState();
            auto &solver = *getEquationSolver();

            switch (dev.get_nThreads()) {
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


    throw "Bad VoidBuilder";
}
*/

