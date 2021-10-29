#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "NumericParams.h"

#include "Studios/CoreMath/Numerics/Output/OutputManager.h"

#include "Equation/Equation.h"

#include "Equation/BoundaryConditions.h"

#include "Method/Method.h"
#include "Method/Method-RK.h"

#include <Studios/Backend/Program.h>
#include <Studios/Tools/BenchmarkHistogram.h>


#define GENERATE_FOR_NTHREADS(N) \
case (N): \
stepper = new StepperRK<N, FIELD_STATE_TYPE>(dPhi); \
break;


class NumericalIntegration : public Program {

    Stepper *stepper;
    OutputManager *outputManager;

    NumericalIntegration(const void *dPhi, OutputManager *outputManager);

public:
    template <class FIELD_STATE_TYPE>
    static NumericalIntegration* New(const void *dPhi, OutputManager *outputManager) {
        auto *instance = new NumericalIntegration(dPhi, outputManager);

        const unsigned int numThreads = Allocator::getInstance().getDevice().get_nThreads();

        Stepper *stepper = nullptr;

        switch (numThreads) {
            GENERATE_FOR_NTHREADS(1);
            GENERATE_FOR_NTHREADS(2);
            GENERATE_FOR_NTHREADS(3);
            GENERATE_FOR_NTHREADS(4);
            GENERATE_FOR_NTHREADS(5);
            GENERATE_FOR_NTHREADS(6);
            GENERATE_FOR_NTHREADS(7);
            GENERATE_FOR_NTHREADS(8);
            GENERATE_FOR_NTHREADS(9);
            GENERATE_FOR_NTHREADS(10);
            GENERATE_FOR_NTHREADS(11);
            GENERATE_FOR_NTHREADS(12);
            GENERATE_FOR_NTHREADS(13);
            GENERATE_FOR_NTHREADS(14);
            GENERATE_FOR_NTHREADS(15);
            GENERATE_FOR_NTHREADS(16);
            default:
                throw "Number of threads must be <= 16.";
        }

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
