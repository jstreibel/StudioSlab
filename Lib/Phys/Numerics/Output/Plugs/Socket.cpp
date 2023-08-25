//
// Created by joao on 10/8/21.
//

#include "Socket.h"
#include "Base/Tools/Log.h"


Numerics::OutputSystem::Socket::Socket(const NumericConfig &params, Str name, int nStepsInterval, Str description)
: intervalStepsBetweenOutputs(nStepsInterval), params(params), name(name), description(description), nextRecStep(1) {      }


auto Numerics::OutputSystem::Socket::getLastSimTime()  -> Real { return lastData.getSimTime(); }

auto Numerics::OutputSystem::Socket::getnSteps() const -> int { return intervalStepsBetweenOutputs; }

auto Numerics::OutputSystem::Socket::setnSteps(int n)  -> void {
    intervalStepsBetweenOutputs = n >= 1 ? n : 1;
}

auto Numerics::OutputSystem::Socket::computeNextRecStep(PosInt currStep) -> size_t {
    if (nextRecStep > currStep)
        return nextRecStep;

    fix n = (Real)intervalStepsBetweenOutputs;
    fix m = (Real)(currStep+1);

    nextRecStep = (int)(n*std::ceil(m/n));

    return nextRecStep;
}

auto Numerics::OutputSystem::Socket::shouldOutput(const Real t, const long unsigned timestep) -> bool {
    (void)t;

#if SHOULD_OUTPUT___MODE == INT_BASED
    return ! (timestep%intervalStepsBetweenOutputs);
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




