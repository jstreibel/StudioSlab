


#include "Integrator.h"

#include <sched.h>
#include <sstream>

#define ATTEMP_REALTIME false

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
    bool activeSteps = args!= nullptr;

    if(activeSteps) {
        for(auto i=0; i<nSteps; ++i){
            histogram.startMeasure();
            stepper->step(dt, 1);
            histogram.storeMeasure(nSteps);

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
    /* Aqui pegamos o campo toda santa vez que queremos fazer output dele.
     * Isso eh feito para que, quando o campo eh integrado na GPU, ele possa
     * ser trazido de la. Quando integrado na CPU, isso nao acontece.
     */
    return OutputPacket(stepper->getFieldState(), stepper->getSpaces(), steps, getSimulationTime());;
}

void NumericalIntegration::runFullIntegration()
{
    size_t n = Numerics::Allocator::getInstance().getNumericParams().getn();

    while(steps < n){
        const size_t nStepsUntilNextOutput = outputManager->computeNStepsToNextOutput(steps);

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

floatt NumericalIntegration::getSimulationTime() { return floatt(getSteps()) * dt; }

const BenchmarkHistogram &NumericalIntegration::getHistogram() const {
    return histogram;
}
