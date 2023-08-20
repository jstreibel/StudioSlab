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

    // Log::Debug() << Log::FGGreen << "Socket::setnSteps(" << n << ")" << Log::Flush;

    // computeNextRecStep();
}

auto Numerics::OutputSystem::Socket::computeNextRecStep(PosInt currStep) -> size_t {
    fix n = (Real)intervalStepsBetweenOutputs;
    fix m = (Real)(currStep+1);

    if(true) nextRecStep = (int)(n*std::ceil(m/n));

    // Log::Debug() << name << " --> nextRecStep = " << nextRecStep << Log::Flush;

    return nextRecStep > 0 ? nextRecStep : intervalStepsBetweenOutputs;
}

auto Numerics::OutputSystem::Socket::shouldOutput(const Real t, const long unsigned timestep) -> bool {

#if SHOULD_OUTPUT___MODE == INT_BASED
    (void)t;

    if(true){
        // Log::Debug() << name << "::shouldOutput --> timestep = " << timestep << " --> nextRecStep = " << nextRecStep << Log::Flush;
        return ! (timestep%intervalStepsBetweenOutputs);
    }
    else
    {
        Log::Debug() << name << "::shouldOutput --> timestep = " << timestep << " --> nextRecStep = " << nextRecStep << Log::Flush;

        if (nextRecStep > timestep)
            return false;

        nextRecStep = (int) timestep + intervalStepsBetweenOutputs;

        return true;
    }

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




