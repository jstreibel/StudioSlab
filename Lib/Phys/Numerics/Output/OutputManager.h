#ifndef OUTPUTMANAGER_H
#define OUTPUTMANAGER_H


#include "Mappings/BCBuilder.h"
#include "Phys/Numerics/Output/Plugs/Plug.h"
#include "Plugs/Plug.h"
#include "Phys/Numerics/Output/Plugs/Plug.h"


class OutputManager {

protected:

	std::vector<Numerics::OutputSystem::Plug*> outputs;
	std::vector<Numerics::OutputSystem::Plug*> myOutputs;


public:

    OutputManager();
	~OutputManager();

	/**
	 * This parameter comes in by reference because copying it can be very heavy. OTOH it is treated as if it is
	 * allocated on the HEAP.
	 *
	 * @param infoVolatile the information to output.
	 */
    void output(OutputPacket &infoVolatile);

    void notifyIntegrationFinished(const OutputPacket &info);

    /****** QUERY ******/
    auto computeNStepsToNextOutput(PosInt currStep) -> PosInt;

    /****** INPUT/OUTPUT ******/
    void addOutputChannel(Numerics::OutputSystem::Plug *out, bool keepTrack = true);


private:
    const size_t maxSteps;


};

#endif // OUTPUTMANAGER_H
