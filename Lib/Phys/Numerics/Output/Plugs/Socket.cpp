//
// Created by joao on 10/8/21.
//

#include "Socket.h"


Numerics::OutputSystem::Socket::Socket(const NumericConfig &params, Str name, int nStepsInterval, Str description)
: nSteps(nStepsInterval), params(params), name(name), description(description), nextRecStep(1) {      }


auto Numerics::OutputSystem::Socket::getLastSimTime()  -> Real { return lastData.getSimTime(); }

auto Numerics::OutputSystem::Socket::getnSteps() const -> int { return nSteps; }

auto Numerics::OutputSystem::Socket::setnSteps(int n)  -> void {
    nSteps = n>=1 ? n : 1;
    computeNextRecStep();
}

auto Numerics::OutputSystem::Socket::computeNextRecStep() -> size_t {
    const size_t lastStep = lastData.getSteps();

    nextRecStep = lastStep+size_t(nSteps);

    return nextRecStep;
}

auto Numerics::OutputSystem::Socket::shouldOutput(const Real t, const long unsigned timestep) -> bool {

#if SHOULD_OUTPUT___MODE == INT_BASED
    (void)t;

    if(nextRecStep>timestep)
        return false;

    nextRecStep = timestep + nSteps;

    return true;

#elif SHOULD_OUTPUT___MODE == FLOAT_BASED
    return abs(T-lastT) > abs(recDT);
#endif

    throw "Boundary not implemented.";
}

void Numerics::OutputSystem::Socket::output(const OutputPacket &outData){
    handleOutput(outData);
    lastData = outData;
}

auto Numerics::OutputSystem::Socket::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool {
    return true;
}

auto Numerics::OutputSystem::Socket::getDescription() const -> Str { return description; }
auto Numerics::OutputSystem::Socket::getName() const -> Str { return name; }




