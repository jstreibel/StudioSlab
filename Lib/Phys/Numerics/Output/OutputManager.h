#ifndef OUTPUTMANAGER_H
#define OUTPUTMANAGER_H

#include "Plugs/Socket.h"

class OutputManager {

protected:

	std::vector<Numerics::OutputSystem::Socket*> outputs;
	std::vector<Numerics::OutputSystem::Socket*> myOutputs;

	const NumericParams &params;

public:

    OutputManager(const NumericParams &params);
	~OutputManager();

	/**
	 * This parameter comes in by reference because copying it can be very heavy. OTOH it is treated as if it is
	 * allocated on the HEAP.
	 *
	 * @param infoVolatile the information to output.
	 */
    void output(OutputPacket &infoVolatile, bool force=false);

    void notifyIntegrationFinished(const OutputPacket &info);

    /****** QUERY ******/
    auto computeNStepsToNextOutput(PosInt currStep) -> PosInt;

    /****** INPUT/OUTPUT ******/
    void addOutputChannel(Numerics::OutputSystem::Socket *out, bool keepTrack = true);


private:
    const size_t maxSteps;


};

#endif // OUTPUTMANAGER_H
