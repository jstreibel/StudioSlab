


#include "Integrator.h"
#include "Base/Tools/Log.h"

#define ATTEMP_REALTIME false
#if ATTEMP_REALTIME
#include <sched.h>
#include <sstream>
#endif



NumericalIntegration::~NumericalIntegration()
{
    Log::Note() << "Avg. integration time: " << simTimeHistogram << Log::Flush;
}

void NumericalIntegration::cycle(CycleOptions options) {
    const auto &p = simBuilder.getNumericParams();
    if(getSimulationTime() >= p.gett() && !simBuilder.getNumericParams().shouldForceOverstepping()) return;

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
}

OutputPacket NumericalIntegration::getOutputInfo(){
    return OutputPacket(stepper->getCurrentState(), stepper->getSpaces(), steps, getSimulationTime());;
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
    auto &p = simBuilder.getNumericParams();
    size_t n = p.getn();

    while(steps < n && _cycleUntilOutput());

    // Para cumprir com os steps quebrados faltantes:
    if(steps < n) cycle(n - steps);

    outputManager->notifyIntegrationFinished(getOutputInfo());
}

bool NumericalIntegration::_cycleUntilOutput() {
    size_t nCyclesToNextOutput = outputManager->computeNStepsToNextOutput(steps);

    if(nCyclesToNextOutput == 0) return false;

    _cycle(nCyclesToNextOutput);

    return true;
}

void NumericalIntegration::output(bool force){
    OutputPacket info = getOutputInfo();
    outputManager->output(info, force);
}

size_t NumericalIntegration::getSteps() const { return steps; }

inline floatt NumericalIntegration::getSimulationTime() { return floatt(getSteps()) * dt; }

const BenchmarkHistogram &NumericalIntegration::getHistogram() const {
    return simTimeHistogram;
}

void NumericalIntegration::doForceOverStepping() {
    Log::Error("NumericalIntegration") << " doForceOverstepping function is deactivated and won't respond.";
}
