


#include "Integrator.h"
#include "Common/Log/Log.h"

#define ATTEMP_REALTIME false
#if ATTEMP_REALTIME
#include <sched.h>
#include <sstream>
#endif

NumericalIntegration::NumericalIntegration(OutputManager *outputManager)
    : outputManager(outputManager),
      dt(Numerics::Allocator::GetInstance().getNumericParams().getdt()),
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
            Log::Error() << "Couldn't set realtime scheduling: " << std::strerror(errno) << Log::Flush;
        } else {
            Log::Info() << "Program running with realtime priority." << Log::Flush;
        }

    }
#endif

}

NumericalIntegration::~NumericalIntegration()
{
    Log::Note() << "Avg. integration time:" << Log::Flush;
    std::cout << simTimeHistogram;

    Log::Note() << "Avg. non-integration time:" << Log::Flush;
    std::cout << nonSimTimeHistogram;
}

void NumericalIntegration::cycle(CycleOptions options) {
    nonSimTimeHistogram.storeMeasure();
    const auto &p = Numerics::Allocator::GetInstance().getNumericParams();
    if(getSimulationTime() >= p.gett() && !forceOverStepping) return;

    switch (options.cycleOption){
        case CycleOptions::Cycle_nCycles:
            _cycle(options.nCycles);
            break;
        case CycleOptions::CycleUntilOutput:
            _cycleUntilOutput();
            break;
        case CycleOptions::cycleCycleUntilFinished:
            _runFullIntegration();
            break;
    }

    nonSimTimeHistogram.startMeasure();
}

OutputPacket NumericalIntegration::getOutputInfo(){
    return OutputPacket(stepper->getFieldState(), stepper->getSpaces(), steps, getSimulationTime());;
}

void NumericalIntegration::_cycle(size_t nCycles) {
    simTimeHistogram.startMeasure();
    stepper->step(dt, nCycles);
    simTimeHistogram.storeMeasure(nCycles);

    steps += nCycles;

    output();
}

void NumericalIntegration::_runFullIntegration()
{
    auto &p = Numerics::Allocator::GetInstance().getNumericParams();
    size_t n = p.getn();

    while(steps < n && _cycleUntilOutput());

    // Para cumprir com os steps quebrados faltantes:
    if(steps < n) cycle(n - steps);

    outputManager->notifyIntegrationFinished(getOutputInfo(), p);
}

bool NumericalIntegration::_cycleUntilOutput() {
    size_t nCyclesToNextOutput = outputManager->computeNStepsToNextOutput(steps);

    if(nCyclesToNextOutput == 0) return false;

    _cycle(nCyclesToNextOutput);

    return true;
}

void NumericalIntegration::output(){
    OutputPacket info = getOutputInfo();
    outputManager->output(info);
}

size_t NumericalIntegration::getSteps() const { return steps; }

inline floatt NumericalIntegration::getSimulationTime() { return floatt(getSteps()) * dt; }

const BenchmarkHistogram &NumericalIntegration::getHistogram() const {
    return simTimeHistogram;
}

void NumericalIntegration::doForceOverStepping() {forceOverStepping = true; }
