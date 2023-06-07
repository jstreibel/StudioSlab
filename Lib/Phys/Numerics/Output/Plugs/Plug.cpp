//
// Created by joao on 10/8/21.
//

#include "Plug.h"


Numerics::OutputSystem::Plug::Plug(String name, int nStepsInterval, String description)
: nStepsBetweenRecordings(nStepsInterval), name(name), description(description) {      }


auto Numerics::OutputSystem::Plug::getLastSimTime() -> Real { return lastData.getSimTime(); }

auto Numerics::OutputSystem::Plug::getNSteps() const -> int { return nStepsBetweenRecordings; }

auto Numerics::OutputSystem::Plug::computeNextRecStep() -> size_t {
    const size_t lastStep = lastData.getSteps();

    return lastStep+size_t(nStepsBetweenRecordings);
}

auto Numerics::OutputSystem::Plug::shouldOutput(const Real t, const long unsigned timestep) -> bool {

#if SHOULD_OUTPUT___MODE == INT_BASED
    (void)t;
    return ! bool(timestep%unsigned(nStepsBetweenRecordings));
#elif SHOULD_OUTPUT___MODE == FLOAT_BASED
    return abs(T-lastT) > abs(recDT);
#endif

    throw "Boundary not implemented.";
}

void Numerics::OutputSystem::Plug::output(const OutputPacket &outData){
    _out(outData);
    lastData = outData;
}

auto Numerics::OutputSystem::Plug::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool {
    return true; }

auto Numerics::OutputSystem::Plug::getDescription() const -> String { return description; }
auto Numerics::OutputSystem::Plug::getName() const -> String { return name; }
