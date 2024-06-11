


#include "NumericTask.h"
#include "Core/Tools/Log.h"

#define ATTEMP_REALTIME false
#if ATTEMP_REALTIME
#include <sched.h>
#include <sstream>
#endif

namespace Slab::Math {

    NumericTask::~NumericTask() {
        Log::Note() << "Avg. integration time: " << benchmarkData << Log::Flush;
    }

    OutputPacket NumericTask::getOutputInfo() {
        auto currentState = stepper->getCurrentState();

        return {currentState, stepsConcluded, getSimulationTime()};
    }

    bool NumericTask::_cycle(size_t nCycles) {
        if(forceStopFlag) return false;

        benchmarkData.startMeasure();
        stepper->step(dt, nCycles);
        benchmarkData.storeMeasure((int) nCycles);

        stepsConcluded += nCycles;

        fix forceOutput = stepsConcluded >= totalSteps;

        output(forceOutput);

        return true;
    }

    bool NumericTask::_cycleUntilOutputOrFinish() {
        size_t nCyclesToNextOutput = outputManager->computeNStepsToNextOutput(stepsConcluded);

        if (nCyclesToNextOutput > 50000) {
            Log::WarningImportant() << "Huge nCyclesToNextOutput: " << nCyclesToNextOutput << Log::Flush;
        }

        if (nCyclesToNextOutput == 0) {
            return false;
        }

        return _cycle(nCyclesToNextOutput);
    }

    void NumericTask::output(bool force) {
        OutputPacket info = getOutputInfo();
        outputManager->output(info, force);
    }

    size_t NumericTask::getSteps() const { return stepsConcluded; }

    inline floatt NumericTask::getSimulationTime() const { return floatt(getSteps()) * dt; }

    const BenchmarkData &NumericTask::getBenchmarkData() const {
        return benchmarkData;
    }

    TaskStatus NumericTask::run() {
        auto &p = numericalRecipe.getNumericParams();
        size_t n = p.getn();

        while (!forceStopFlag && stepsConcluded < n && _cycleUntilOutputOrFinish());

        if(forceStopFlag)
            return Aborted;

        // Para cumprir com os steps quebrados faltantes:
        if (stepsConcluded < n) if(!_cycle(n - stepsConcluded))
            return InternalError;

        outputManager->notifyIntegrationFinished(getOutputInfo());

        return Success;
    }

    void NumericTask::abort() {
        forceStopFlag = true;
    }


}