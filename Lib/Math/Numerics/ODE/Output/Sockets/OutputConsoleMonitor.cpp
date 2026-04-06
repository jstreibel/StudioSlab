#include "OutputConsoleMonitor.h"
#include "Core/Tools/Log.h"


namespace Slab::Math {

    FOutputConsoleMonitor::FOutputConsoleMonitor(const CountType total_steps, CountType steps_interval)
    : FOutputChannel("Console monitor output", static_cast<int>(steps_interval))
    , total_steps(total_steps) {

    }

    bool FOutputConsoleMonitor::NotifyIntegrationHasFinished(const FOutputPacket &theVeryLastOutputInformation) {
        // Isso aqui eh para aparecer o 100% completo (se nao fica uns quebrados).
        FOutputPacket dummyInfo = FOutputPacket(nullptr, total_steps);

        this->HandleOutput(dummyInfo);
        return true;
    }

    void FOutputConsoleMonitor::HandleOutput(const FOutputPacket &outputInfo) {
        static Vector<DevFloat> measures;
        auto elTime = timer.GetElapsedTimeSeconds();

        auto n = total_steps;
        auto currn = outputInfo.GetSteps();
        static auto lastn = currn;

        Core::FLog::Info() << Core::FLog::Flush;
        Core::FLog::Info() << (100 * static_cast<DevFloat>(currn) / total_steps) << "% done" << Core::FLog::Flush;
        Core::FLog::Info() << "Step " << outputInfo.GetSteps() << "/" << n << Core::FLog::Flush;

        auto expectedFinish = (DevFloat)NAN;
        if (lastn != currn) {
            DevFloat stepsPerSec;

            measures.emplace_back(elTime);

            auto avg = 0.0;
            const auto MAX_AVG_SAMPLES = 20;
            auto count = measures.size();
            auto countMin = count > MAX_AVG_SAMPLES ? count - MAX_AVG_SAMPLES : 0;
            auto total = count - countMin;
            for (int index = (int)countMin; index < count; index++) avg += measures[index];
            avg /= (DevFloat)total;

            stepsPerSec = (DevFloat)(currn - lastn) / avg;

            expectedFinish = (DevFloat)(n - currn) / stepsPerSec;

            Core::FLog::Info() << "Avg " << stepsPerSec << " steps/s in last " << total << " measures" << Core::FLog::Flush;
            Core::FLog::Info() << "El time since last step: " << elTime << "s" << Core::FLog::Flush;
            Core::FLog::Info() << "Expected finish in " << Core::FLog::FGBlue << int(expectedFinish) / 60 << "m "
                        << int(expectedFinish) % 60 << "s" << Core::FLog::Flush;

            // this->setnSteps(stepsPerSec);
        }

        lastn = currn;

        timer.Reset();
    }


}
