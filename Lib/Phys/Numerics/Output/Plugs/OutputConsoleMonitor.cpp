#include "OutputConsoleMonitor.h"
#include "Phys/Numerics/Output/Plugs/Socket.h"
#include "Common/Log/Log.h"

#include <Phys/Numerics/Allocator.h>

OutputConsoleMonitor::OutputConsoleMonitor(const NumericParams &params, const int n_steps)
    : Socket(params, "Console monitor output", n_steps),
      maxT(Numerics::Allocator::GetInstance().getNumericParams().gett())
{

}

bool OutputConsoleMonitor::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation)
{
	// Isso aqui eh para aparecer o 100% completo (se nao fica uns quebrados).
    OutputPacket dummyInfo = OutputPacket(nullptr, DiscreteSpacePair(nullptr, nullptr),
                                          Numerics::Allocator::GetInstance().getNumericParams().getn(), maxT);

    this->_out(dummyInfo);
    return true;
}

void OutputConsoleMonitor::_out(const OutputPacket &outputInfo)
{
    auto n = params.getn();

    Log::Info() << "Step " << outputInfo.getSteps() << "/" << n << "; "
                << "Sim time " << outputInfo.getSimTime() << "/" << maxT << Log::Flush;

}
