
#include "OutputManager.h"
#include "Phys/Numerics/Output/Plugs/Socket.h"
#include "Common/Log/Log.h"

#include <Phys/Numerics/Allocator.h>


OutputManager::OutputManager() : maxSteps(Numerics::Allocator::GetInstance().getNumericParams().getn()) { }

OutputManager::~OutputManager() = default; // No need to destroy output objects in vectors;

void OutputManager::output(OutputPacket &infoVolatile)
{
    const Real t = infoVolatile.getSimTime();
    const size_t steps = infoVolatile.getSteps();

    for(auto *out : outputs) {
        auto shouldOutput = out->shouldOutput(t, steps);
        if (shouldOutput)
            out->output(infoVolatile);
    }

}


auto OutputManager::computeNStepsToNextOutput(PosInt currStep) -> PosInt
{
    size_t nSteps = maxSteps;

    for(auto &output : outputs){
        const size_t nextRecStep = output->computeNextRecStep();
        if(nextRecStep < nSteps) nSteps = nextRecStep;
    }

    return nSteps-currStep;
}

void OutputManager::addOutputChannel(Numerics::OutputSystem::Socket *out, bool keepTrack)
{
    outputs.push_back(out);
    if(keepTrack) myOutputs.push_back(out);

    Log::Info() << "Output manager added \"" << out->getName() << "\" output channel. Updates "
                   "every " << out->getnSteps() << " sim steps." << Log::Flush;
}

void OutputManager::notifyIntegrationFinished(const OutputPacket &theVeryLastOutputInformation) {
    Log::Success() << "Simulation finished. Total time steps: "
                   << theVeryLastOutputInformation.getSteps() << Log::Flush;

    for(auto output : myOutputs)
    {
        if(!output->notifyIntegrationHasFinished(theVeryLastOutputInformation))
            Log::Error() << "Error while finishing " << output->getName() << "..." << Log::Flush;

        Log::Info() << "Finished output channel '" << output->getName() << "'." << Log::Flush;
    }
}
