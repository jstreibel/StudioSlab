#include "OutputConsoleMonitor.h"
#include "Math/Numerics/Output/Plugs/Socket.h"
#include "Core/Tools/Log.h"


namespace Slab::Math {

    OutputConsoleMonitor::OutputConsoleMonitor(Count total_steps, Real max_t, Real r)
    : Socket("Console monitor output", int(1000./r))
    , max_t(max_t), total_steps(total_steps) {

    }

    bool OutputConsoleMonitor::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) {
        // Isso aqui eh para aparecer o 100% completo (se nao fica uns quebrados).
        OutputPacket dummyInfo = OutputPacket(nullptr, total_steps, max_t);

        this->handleOutput(dummyInfo);
        return true;
    }

    void OutputConsoleMonitor::handleOutput(const OutputPacket &outputInfo) {
        static Vector<Real> measures;
        auto elTime = timer.getElTime_sec();

        auto n = total_steps;
        auto currn = outputInfo.getSteps();
        static auto lastn = currn;

        auto t = outputInfo.getSimTime();
        Core::Log::Info() << Core::Log::Flush;
        Core::Log::Info() << (100 * t / max_t) << "% done" << Core::Log::Flush;
        Core::Log::Info() << "Step " << outputInfo.getSteps() << "/" << n << Core::Log::Flush;
        Core::Log::Info() << "t = " << t << "/" << max_t << Core::Log::Flush;

        auto expectedFinish = (Real)NAN;
        if (lastn != currn) {
            Real stepsPerSec;

            measures.emplace_back(elTime);

            auto avg = 0.0;
            const auto MAX_AVG_SAMPLES = 20;
            auto count = measures.size();
            auto countMin = count > MAX_AVG_SAMPLES ? count - MAX_AVG_SAMPLES : 0;
            auto total = count - countMin;
            for (int index = (int)countMin; index < count; index++) avg += measures[index];
            avg /= (Real)total;

            stepsPerSec = (Real)(currn - lastn) / avg;

            expectedFinish = (Real)(n - currn) / stepsPerSec;

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