


#include "Integrator.h"
#include "Core/Tools/Log.h"

#define ATTEMP_REALTIME false
#if ATTEMP_REALTIME
#include <sched.h>
#include <sstream>
#endif

namespace Slab::Math {

    NumericalIntegration::~NumericalIntegration() {
        Log::Note() << "Avg. integration time: " << simTimeHistogram << Log::Flush;
    }

    OutputPacket NumericalIntegration::getOutputInfo() {
        return {stepper->getCurrentState(), stepper->getSpaces(), steps, getSimulationTime()};
    }

    bool NumericalIntegration::_cycle(size_t nCycles) {

        simTimeHistogram.startMeasure();
        stepper->step(dt, nCycles);
        simTimeHistogram.storeMeasure((int) nCycles);

        steps += nCycles;

        static fix maxSteps = simBuilder.getNumericParams().getn();
        fix forceOutput = steps >= maxSteps;

        output(forceOutput);

        return true;
    }

    bool NumericalIntegration::_cycleUntilOutputOrFinish() {
        size_t nCyclesToNextOutput = outputManager->computeNStepsToNextOutput(steps);

        if (nCyclesToNextOutput > 50000) {
            Log::WarningImportant() << "Huge nCyclesToNextOutput: " << nCyclesToNextOutput << Log::Flush;
        }

        if (nCyclesToNextOutput == 0) {
            return false;
        }

        _cycle(nCyclesToNextOutput);

        return true;
    }

    void NumericalIntegration::output(bool force) {
        OutputPacket info = getOutputInfo();
        outputManager->output(info, force);
    }

    size_t NumericalIntegration::getSteps() const { return steps; }

    inline floatt NumericalIntegration::getSimulationTime() const { return floatt(getSteps()) * dt; }

    const BenchmarkHistogram &NumericalIntegration::getHistogram() const {
        return simTimeHistogram;
    }

    bool NumericalIntegration::run() {
        auto &p = simBuilder.getNumericParams();
        size_t n = p.getn();

        while (!forceStopFlag && steps < n && _cycleUntilOutputOrFinish());

        // Para cumprir com os steps quebrados faltantes:
        if (steps < n) _cycle(n - steps);

        outputManager->notifyIntegrationFinished(getOutputInfo());

        return true;
    }

    void NumericalIntegration::forceStop() {
        forceStopFlag = true;
    }


}