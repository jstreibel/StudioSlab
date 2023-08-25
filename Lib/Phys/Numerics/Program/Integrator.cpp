


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

bool NumericalIntegration::cycle(CycleOptions options) {
    const auto &p = simBuilder.getNumericParams();

    if(getSimulationTime() >= p.gett() && !p.shouldForceOverstepping()){
        if(!integrationFinished){
            outputManager->notifyIntegrationFinished(getOutputInfo());
            integrationFinished = true;
        }

        return false;
    }

    switch (options.cycleOption){
        case CycleOptions::Cycle_nCycles:
            return _cycle(options.nCycles);
        case CycleOptions::CycleUntilOutput:
            return _cycleUntilOutputOrFinish();
        case CycleOptions::cycleCycleUntilFinished:
            return _runFullIntegration();
    }

    return false;
}

OutputPacket NumericalIntegration::getOutputInfo(){
    return {stepper->getCurrentState(), stepper->getSpaces(), steps, getSimulationTime()};
}

bool NumericalIntegration::_cycle(size_t nCycles) {

    simTimeHistogram.startMeasure();
    stepper->step(dt, nCycles);
    simTimeHistogram.storeMeasure((int)nCycles);

    steps += nCycles;

    static fix maxSteps = simBuilder.getNumericParams().getn();
    fix forceOutput = steps >= maxSteps;

    output(forceOutput);

    return true;
}

bool NumericalIntegration::_runFullIntegration()
{
    auto &p = simBuilder.getNumericParams();
    size_t n = p.getn();

    while(steps < n && _cycleUntilOutputOrFinish());

    // Para cumprir com os steps quebrados faltantes:
    if(steps < n) _cycle(n - steps);

    outputManager->notifyIntegrationFinished(getOutputInfo());

    return true;
}

bool NumericalIntegration::_cycleUntilOutputOrFinish() {
    size_t nCyclesToNextOutput = outputManager->computeNStepsToNextOutput(steps);

    if (nCyclesToNextOutput > 50000) {
        Log::WarningImportant() << "Huge nCyclesToNextOutput: " << nCyclesToNextOutput << Log::Flush;
    }

    if (nCyclesToNextOutput == 0) {
        Log::WarningImportant() << "No more cycles to next output" << Log::Flush;
        return false;
    }

    _cycle(nCyclesToNextOutput);

    return true;
}

void NumericalIntegration::output(bool force){
    OutputPacket info = getOutputInfo();
    outputManager->output(info, force);
}

size_t NumericalIntegration::getSteps() const { return steps; }

inline floatt NumericalIntegration::getSimulationTime() const { return floatt(getSteps()) * dt; }

const BenchmarkHistogram &NumericalIntegration::getHistogram() const {
    return simTimeHistogram;
}

void NumericalIntegration::doForceOverStepping() {
    Log::Error("NumericalIntegration") << " doForceOverstepping function is deactivated and won't respond.";
}
