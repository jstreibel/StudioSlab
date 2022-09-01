#include "OutputConsoleMonitor.h"

#include <Phys/Numerics/Allocator.h>

OutputConsoleMonitor::OutputConsoleMonitor(const int n_steps, bool doCarrierReturn)
    : OutputChannel(n_steps), timer(), maxT(Allocator::getInstance().getNumericParams().gett()),
      steppingChar(doCarrierReturn ? '\r' : '\n')
{
    timer.resume();
}

bool OutputConsoleMonitor::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation)
{
	// Isso aqui eh para aparecer o 100% completo (se nao fica uns quebrados).
    OutputPacket dummyInfo = OutputPacket(nullptr, DiscreteSpacePair(nullptr, nullptr),
                                      Allocator::getInstance().getNumericParams().getn(), maxT);

    this->_out(dummyInfo);
    return true;
}

void OutputConsoleMonitor::_out(const OutputPacket &outputInfo)
{

    auto &params = Allocator::getInstance().getNumericParams();
    auto n = params.getn();

    std::cout << steppingChar << "Step " << outputInfo.getSteps() << "/" << n << "   ---   " <<
                 " Sim time " << outputInfo.getT() << "/" << maxT << "               " << std::flush;



    if(0) {
        const double percCompl = abs(outputInfo.getT()/maxT);

        size_t outputResT = Allocator::getInstance().getNumericParams().getn();

        std::cout << steppingChar << "[";
        const size_t n = outputResT / size_t(nStepsBetweenRecordings);
        const size_t curr = outputInfo.getSteps() / size_t(nStepsBetweenRecordings);

        size_t i = 0;
        for (; i < curr; ++i) std::cout << ".";
        for (size_t j = i; j < n; ++j) std::cout << " ";
        std::cout << "] " << std::setprecision(3) << percCompl * 100 << "%     " << std::flush;
    }
}
