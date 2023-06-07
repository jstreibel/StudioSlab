


#include "Integrator.h"

#define ATTEMP_REALTIME false
#if ATTEMP_REALTIME
#include <sched.h>
#include <sstream>
#endif

NumericalIntegration::NumericalIntegration(const void *dPhi, OutputManager *outputManager)
    : outputManager(outputManager),
      dt(Numerics::Allocator::getInstance().getNumericParams().getdt()),
      steps(0)
{
#if ATTEMP_REALTIME
    {
        // Declare a sched_param struct to hold the scheduling parameters.
        sched_param param;

        // Set the priority value in the sched_param struct.
        param.sched_priority = sched_get_priority_max(SCHED_FIFO);

        // Set the scheduling policy and priority of the current process.
        int ret = sched_setscheduler(0, SCHED_FIFO, &param);
        if (ret == -1) {
            std::cout << "Error setting realtime scheduling: " << std::strerror(errno) << std::endl;
        } else {
            std::cout << "Program running with realtime priority." << std::endl;
        }

    }
#endif

}

NumericalIntegration::~NumericalIntegration()
{    std::cout << histogram;     }

void NumericalIntegration::step(PosInt nSteps, void *args) {
    const auto &p = Numerics::Allocator::getInstance().getNumericParams();
    if(getSimulationTime() >= p.gett() && !forceOverStepping) return;

    bool activeSteps = args != nullptr;

    if(activeSteps) {
        for(auto i=0; i<nSteps; ++i){
            histogram.startMeasure();
            stepper->step(dt, 1);
            histogram.storeMeasure(1);

            steps++;

            output();
        }
    } else {
        histogram.startMeasure();
        stepper->step(dt, nSteps);
        histogram.storeMeasure(nSteps);

        steps += nSteps;

        output();
    }


}

OutputPacket NumericalIntegration::getOutputInfo(){
    return OutputPacket(stepper->getFieldState(), stepper->getSpaces(), steps, getSimulationTime());;
}

void NumericalIntegration::runFullIntegration()
{
    size_t n = Numerics::Allocator::getInstance().getNumericParams().getn();

    while(steps < n){
        const size_t nStepsUntilNextOutput = outputManager->computeNStepsToNextOutput(steps);

        if(nStepsUntilNextOutput == 0) break;

        step(nStepsUntilNextOutput);
    }

    // Para cumprir com os steps quebrados:
    if(steps < n) step(n-steps);

    outputManager->notifyIntegrationFinished(getOutputInfo());
}

void NumericalIntegration::output(){
    OutputPacket info = getOutputInfo();
    outputManager->output(info);
}

size_t NumericalIntegration::getSteps() const { return steps; }

inline floatt NumericalIntegration::getSimulationTime() { return floatt(getSteps()) * dt; }

const BenchmarkHistogram &NumericalIntegration::getHistogram() const {
    return histogram;
}

void NumericalIntegration::doForceOverStepping() {forceOverStepping = true; }
