#ifndef OUTPUTMANAGER_H
#define OUTPUTMANAGER_H


#include "Fields/Mappings/BCInterface.h"
#include "Phys/Numerics/Output/Channel/OutputChannel.h"


class OutputManager {

protected:

	std::vector<OutputChannel*> outputs;
	std::vector<OutputChannel*> myOutputs;


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
    void addOutputChannel(OutputChannel *out, bool keepTrack = true);


private:
    const size_t maxSteps;


};

#endif // OUTPUTMANAGER_H
