#include "OutputConsoleMonitor.h"
#include "Phys/Numerics/Output/Plugs/Socket.h"
#include "Base/Tools/Log.h"

OutputConsoleMonitor::OutputConsoleMonitor(const NumericParams &params, const Count n_steps_between_calls)
    : Socket(params, "Console monitor output", n_steps_between_calls),
      maxT(params.gett())
{

}

bool OutputConsoleMonitor::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation)
{
	// Isso aqui eh para aparecer o 100% completo (se nao fica uns quebrados).
    OutputPacket dummyInfo = OutputPacket(nullptr, DiscreteSpacePair(nullptr, nullptr),
                                          params.getn(), maxT);

    this->_out(dummyInfo);
    return true;
}

void OutputConsoleMonitor::_out(const OutputPacket &outputInfo)
{
    static std::vector<Real> measures;
    static Timer timer;
    auto elTime = timer.getElTime_sec();

    auto n = params.getn();
    auto currn = outputInfo.getSteps();
    static auto lastn = currn;

    auto t = outputInfo.getSimTime();
    Log::Info() << Log::Flush;
    Log::Info() << (100*t/maxT) << "% done" << Log::Flush;
    Log::Info() << "Step " << outputInfo.getSteps() << "/" << n << Log::Flush;
    Log::Info() << "t = " << t << "/" << maxT << Log::Flush;

    auto stepsPerSec = 0.0;
    auto expectedFinish = NAN;
    if(lastn != currn){
        measures.emplace_back(elTime);

        auto avg = 0.0;
        const auto MAX_AVG_SAMPLES = 20;
        auto count = measures.size();
        auto countMin = count > MAX_AVG_SAMPLES ? count-MAX_AVG_SAMPLES : 0;
        auto total = count-countMin;
        for(int index=countMin; index<count; index++) avg += measures[index];
        avg /= total;

        stepsPerSec = (currn-lastn)/avg;

        expectedFinish = (n-currn)/stepsPerSec;

        Log::Info() << "Avg "<< stepsPerSec << " steps/s in last " << total << " measures" << Log::Flush;
        Log::Info() << "El time since last step: " << elTime << "s" << Log::Flush;
        Log::Info() << "Expected finish in " << Log::FGBlue << int(expectedFinish) / 60 << "m " << int(expectedFinish) % 60 << "s" << Log::Flush;
    }

    lastn = currn;

    timer.reset();
}
