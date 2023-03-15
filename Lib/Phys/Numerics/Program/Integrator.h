#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "NumericParams.h"

#include "Phys/Numerics/Output/OutputManager.h"

#include "Phys/Numerics/Equation/Equation.h"

#include "Phys/Numerics/Equation/BoundaryConditions.h"

#include "Phys/Numerics/Method/Method.h"
#include "Phys/Numerics/Method/Method-RK4.h"
#include "Phys/Numerics/Method/Method-Euler.h"

#include <Studios/Backend/Program.h>
#include <Studios/Tools/BenchmarkHistogram.h>


#define GENERATE_FOR_NTHREADS(STEPPER_TYPE, N) \
case (N): \
stepper = new STEPPER_TYPE<N, FIELD_STATE_TYPE>(dPhi); \
break;


class NumericalIntegration : public Program {

    Method *stepper;
    OutputManager *outputManager;

    NumericalIntegration(const void *dPhi, OutputManager *outputManager);

public:
    enum Methods {Euler, RK4};

    template <class FIELD_STATE_TYPE>
    static NumericalIntegration* New(const void *dPhi, OutputManager *outputManager, Methods theMethod=RK4)
    {
        auto *instance = new NumericalIntegration(dPhi, outputManager);

        const unsigned int numThreads = Allocator::getInstance().getDevice().get_nThreads();

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
        } else if(theMethod==Euler) {
            switch (numThreads) {
                GENERATE_FOR_NTHREADS(StepperEuler, 1);
                GENERATE_FOR_NTHREADS(StepperEuler, 2);
                GENERATE_FOR_NTHREADS(StepperEuler, 3);
                GENERATE_FOR_NTHREADS(StepperEuler, 4);
                GENERATE_FOR_NTHREADS(StepperEuler, 5);
                GENERATE_FOR_NTHREADS(StepperEuler, 6);
                GENERATE_FOR_NTHREADS(StepperEuler, 7);
                GENERATE_FOR_NTHREADS(StepperEuler, 8);
                GENERATE_FOR_NTHREADS(StepperEuler, 9);
                GENERATE_FOR_NTHREADS(StepperEuler, 10);
                GENERATE_FOR_NTHREADS(StepperEuler, 11);
                GENERATE_FOR_NTHREADS(StepperEuler, 12);
                GENERATE_FOR_NTHREADS(StepperEuler, 13);
                GENERATE_FOR_NTHREADS(StepperEuler, 14);
                GENERATE_FOR_NTHREADS(StepperEuler, 15);
                GENERATE_FOR_NTHREADS(StepperEuler, 16);
                default:
                    throw "Number of threads must be between 1 and 16 inclusive.";
            }
        } else throw "Unknown integration method.";

        instance->stepper = stepper;
        instance->output(); // output do estado inicial

        return instance;
    }

    ~NumericalIntegration();

    void step(PosInt nSteps = 1) override;

    OutputPacket getOutputInfo();

public:
    void runFullIntegration() override;
    void output();
    const BenchmarkHistogram& getHistogram() const;

public:
    size_t getSteps() const;
    Real getT();

private:
    Real dt;
    PosInt steps;

    BenchmarkHistogram histogram;

};

#endif // def INTEGRATOR_H
