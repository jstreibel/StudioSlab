//
// Created by joao on 10/8/21.
//

#include "Socket.h"


Numerics::OutputSystem::Socket::Socket(String name, int nStepsInterval, String description)
: nSteps(nStepsInterval), name(name), description(description), nextRecStep(nStepsInterval) {      }


auto Numerics::OutputSystem::Socket::getLastSimTime()  -> Real { return lastData.getSimTime(); }

auto Numerics::OutputSystem::Socket::getnSteps() const -> int { return nSteps; }

auto Numerics::OutputSystem::Socket::setnSteps(int n)  -> void {
    nSteps = n>=1 ? n : 1;
}

auto Numerics::OutputSystem::Socket::computeNextRecStep() -> size_t {
    const size_t lastStep = lastData.getSteps();

    nextRecStep = lastStep+size_t(nSteps);

    return nextRecStep;
}

auto Numerics::OutputSystem::Socket::shouldOutput(const Real t, const long unsigned timestep) -> bool {

#if SHOULD_OUTPUT___MODE == INT_BASED
    (void)t;
    bool should = nextRecStep==timestep;

    nextRecStep += nSteps;

    return should;
#elif SHOULD_OUTPUT___MODE == FLOAT_BASED
    return abs(T-lastT) > abs(recDT);
#endif

    throw "Boundary not implemented.";
}

void Numerics::OutputSystem::Socket::output(const OutputPacket &outData){
    _out(outData);
    lastData = outData;
}

auto Numerics::OutputSystem::Socket::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool {
    return true; }

auto Numerics::OutputSystem::Socket::getDescription() const -> String { return description; }
auto Numerics::OutputSystem::Socket::getName() const -> String { return name; }




