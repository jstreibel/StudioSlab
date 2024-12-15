


#include "NumericTask.h"

#define ATTEMP_REALTIME false
#if ATTEMP_REALTIME
#include <sched.h>
#include <sstream>
#endif

namespace Slab::Math {

    using Core::Log;

    NumericTask::NumericTask(Base::NumericalRecipe &recipe)
            : Task("Numeric Integration"),
              totalSteps(recipe.getNumericConfig()->getn()),
              numericalRecipe(recipe),
              stepper(recipe.buildStepper()),
              outputManager(New<OutputManager>(totalSteps)),
              stepsConcluded(0),
              benchmarkData(recipe.getNumericConfig()->getn()/100){
        auto sockets = recipe.buildOutputSockets();

        for(auto &socket : sockets) outputManager->addOutputChannel(socket);

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

        this->output(FORCE_INITIAL_OUTPUT);
    }

    NumericTask::~NumericTask() {
        Log::Note() << "Avg. integration time: " << benchmarkData << Log::Flush;
    }

    OutputPacket NumericTask::getOutputInfo() {
        auto currentState = stepper->getCurrentState();

        return {currentState, stepsConcluded};
    }

    bool NumericTask::_cycle(size_t nCycles) {
        if(forceStopFlag) return false;

        benchmarkData.startMeasure();
        stepper->step(nCycles);
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

    const Core::BenchmarkData &NumericTask::getBenchmarkData() const {
        return benchmarkData;
    }

    Core::TaskStatus NumericTask::run() {
        numericalRecipe.setupForCurrentThread();

        size_t n = numericalRecipe.getNumericConfig()->getn();

        while (!forceStopFlag && stepsConcluded < n && _cycleUntilOutputOrFinish());

        if(forceStopFlag)                                       return Core::Aborted;

        // Para cumprir com os steps quebrados faltantes:
        if (stepsConcluded < n) if(!_cycle(n - stepsConcluded)) return Core::InternalError;

        outputManager->notifyIntegrationFinished(getOutputInfo());

        return Core::Success;
    }

    void NumericTask::abort() {
        forceStopFlag = true;
    }

}