#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "Core/NumericParams.h"

#include "View/OutputManager.h"

#include "Model/System.h"

#include "Model/BoundaryConditions.h"

#include "Core/Stepper/stepper.h"
#include "Core/Stepper/stepper-rk.h"

#include <Studios/Util/BenchmarkHistogram.h>
#include "Studios/Backend/Program.h"


class NumericalIntegration : public Program {

private:
    Stepper *stepper;
    OutputManager *outputManager;

public:
    NumericalIntegration(const void *dPhi, OutputManager *outputManager);
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
