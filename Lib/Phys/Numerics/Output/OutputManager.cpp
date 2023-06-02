
#include "OutputManager.h"
#include "Phys/Numerics/Output/Plugs/Plug.h"

#include <Phys/Numerics/Allocator.h>


OutputManager::OutputManager() : maxSteps(Numerics::Allocator::getInstance().getNumericParams().getn()) { }

OutputManager::~OutputManager() = default; // No need to destroy output objects in vectors;

void OutputManager::output(OutputPacket &infoVolatile)
{
    const double t = infoVolatile.getSimTime();
    const size_t steps = infoVolatile.getSteps();

    for(auto *out : outputs)
        if(out->shouldOutput(t, steps)) out->output(infoVolatile);

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

void OutputManager::addOutputChannel(Numerics::OutputSystem::Plug *out, bool keepTrack)
{
    outputs.push_back(out);
    if(keepTrack) myOutputs.push_back(out);

    std::cout << "Output manager added \"" << out->getName() << "\" output channel. Updates "
                 "every " << out->getNSteps() << " sim steps." << std::endl;
}

void OutputManager::notifyIntegrationFinished(const OutputPacket &theVeryLastOutputInformation) {
    std::cout << std::endl << std::endl << "Simulation finished. Total time steps: "
         << theVeryLastOutputInformation.getSteps();

    for(auto output : myOutputs)
    {
        std::cout << "\nFinishing " << output->getDescription() << ". ";
        if(!output->notifyIntegrationHasFinished(theVeryLastOutputInformation))
            std::cout << "Error while finishing " << output->getDescription() << "...";
    }
}
