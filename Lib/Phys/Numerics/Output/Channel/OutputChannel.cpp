//
// Created by joao on 10/8/21.
//

#include "OutputChannel.h"


OutputChannel::OutputChannel(String name, int nStepsInterval, String description)
: nStepsBetweenRecordings(nStepsInterval), name(name), description(description) {      }


auto OutputChannel::getLastSimTime() -> double { return lastData.getSimTime(); }

auto OutputChannel::getNSteps() const -> int { return nStepsBetweenRecordings; }

auto OutputChannel::computeNextRecStep() -> size_t {
    const size_t lastStep = lastData.getSteps();

    return lastStep+size_t(nStepsBetweenRecordings);
}

auto OutputChannel::shouldOutput(const double t, const long unsigned timestep) -> bool {

#if SHOULD_OUTPUT___MODE == INT_BASED
    (void)t;
    return ! bool(timestep%unsigned(nStepsBetweenRecordings));
#elif SHOULD_OUTPUT___MODE == FLOAT_BASED
    return abs(T-lastT) > abs(recDT);
#endif

    throw "Boundary not implemented.";
}

void OutputChannel::output(const OutputPacket &outData){
    _out(outData);
    lastData = outData;
}

auto OutputChannel::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool {
    return true; }

auto OutputChannel::getDescription() const -> String { return description; }
auto OutputChannel::getName() const -> String { return name; }
