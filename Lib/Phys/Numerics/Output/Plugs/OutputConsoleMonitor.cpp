#include "OutputConsoleMonitor.h"
#include "Phys/Numerics/Output/Plugs/Plug.h"
#include "Common/Log/Log.h"

#include <Phys/Numerics/Allocator.h>

OutputConsoleMonitor::OutputConsoleMonitor(const int n_steps)
    : Plug("Console monitor output", n_steps),
      maxT(Numerics::Allocator::getInstance().getNumericParams().gett())
{

}

bool OutputConsoleMonitor::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation)
{
	// Isso aqui eh para aparecer o 100% completo (se nao fica uns quebrados).
    OutputPacket dummyInfo = OutputPacket(nullptr, DiscreteSpacePair(nullptr, nullptr),
                                          Numerics::Allocator::getInstance().getNumericParams().getn(), maxT);

    this->_out(dummyInfo);
    return true;
}

void OutputConsoleMonitor::_out(const OutputPacket &outputInfo)
{

    auto &params = Numerics::Allocator::getInstance().getNumericParams();
    auto n = params.getn();

    Log::Info() << "Step " << outputInfo.getSteps() << "/" << n << "; "
                << "Sim time " << outputInfo.getSimTime() << "/" << maxT << Log::Flush;

}
