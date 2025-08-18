#include "OutputConsoleMonitor.h"
#include "Core/Tools/Log.h"


namespace Slab::Math {

    OutputConsoleMonitor::OutputConsoleMonitor(const CountType total_steps, CountType steps_interval)
    : FOutputChannel("Console monitor output", static_cast<int>(steps_interval))
    , total_steps(total_steps) {

    }

    bool OutputConsoleMonitor::NotifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) {
        // Isso aqui eh para aparecer o 100% completo (se nao fica uns quebrados).
        OutputPacket dummyInfo = OutputPacket(nullptr, total_steps);

        this->HandleOutput(dummyInfo);
        return true;
    }

    void OutputConsoleMonitor::HandleOutput(const OutputPacket &outputInfo) {
        static Vector<DevFloat> measures;
        auto elTime = timer.GetElapsedTime_Seconds();

        auto n = total_steps;
        auto currn = outputInfo.GetSteps();
        static auto lastn = currn;

        Core::Log::Info() << Core::Log::Flush;
        Core::Log::Info() << (100 * static_cast<DevFloat>(currn) / total_steps) << "% done" << Core::Log::Flush;
        Core::Log::Info() << "Step " << outputInfo.GetSteps() << "/" << n << Core::Log::Flush;

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

            Core::Log::Info() << "Avg " << stepsPerSec << " steps/s in last " << total << " measures" << Core::Log::Flush;
            Core::Log::Info() << "El time since last step: " << elTime << "s" << Core::Log::Flush;
            Core::Log::Info() << "Expected finish in " << Core::Log::FGBlue << int(expectedFinish) / 60 << "m "
                        << int(expectedFinish) % 60 << "s" << Core::Log::Flush;

            // this->setnSteps(stepsPerSec);
        }

        lastn = currn;

        timer.reset();
    }


}