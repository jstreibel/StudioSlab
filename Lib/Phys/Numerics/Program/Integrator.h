#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "NumericParams.h"

#include "Phys/Numerics/Output/OutputManager.h"

#include "Phys/DifferentialEquations/DifferentialEquation.h"

#include "Phys/DifferentialEquations/BoundaryConditions.h"

#include "Phys/Numerics/Method/Method.h"
#include "Phys/Numerics/Method/Method-RK4.h"
#include "Phys/Numerics/Method/Method-MCBase.h"

#include <Base/Backend/Program.h>
#include <Base/Tools/BenchmarkHistogram.h>


#define GENERATE_FOR_NTHREADS(STEPPER_TYPE, N) \
case (N): \
stepper = new STEPPER_TYPE<N, FIELD_STATE_TYPE>(dPhi); \
break;


class NumericalIntegration : public Program {

    Method *stepper;
    OutputManager *outputManager;

    NumericalIntegration(const void *dPhi, OutputManager *outputManager);

public:
    enum Methods {Montecarlo, RK4};

    template <class FIELD_STATE_TYPE>
    static NumericalIntegration* New(const void *dPhi, OutputManager *outputManager, Methods theMethod=RK4)
    {
        auto *instance = new NumericalIntegration(dPhi, outputManager);

        const unsigned int numThreads = Numerics::Allocator::getInstance().getDevice().get_nThreads();

        Method *stepper = nullptr;

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

        instance->stepper = stepper;
        instance->output(); // output do estado inicial

        return instance;
    }

    ~NumericalIntegration();

    void step(PosInt nSteps = 1, void *args= nullptr) override;

    OutputPacket getOutputInfo();

public:
    void runFullIntegration() override;
    void output();
    const BenchmarkHistogram& getHistogram() const;

public:
    size_t getSteps() const;
    inline Real getSimulationTime();
    void doForceOverStepping();

private:
    Real dt;
    PosInt steps;
    bool forceOverStepping = false;

    BenchmarkHistogram histogram;

};

#endif // def INTEGRATOR_H
