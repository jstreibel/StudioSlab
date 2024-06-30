#include "OutputConsoleMonitor.h"
#include "Math/Numerics/Output/Plugs/Socket.h"
#include "Core/Tools/Log.h"


namespace Slab::Math {

    OutputConsoleMonitor::OutputConsoleMonitor(const NumericConfig &params)
    : Socket(params, "Console monitor output", int(1000./params.getr()))
    , maxT(params.gett()) {

    }

    bool OutputConsoleMonitor::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) {
        // Isso aqui eh para aparecer o 100% completo (se nao fica uns quebrados).
        OutputPacket dummyInfo = OutputPacket(nullptr,  params.getn(), maxT);

        this->handleOutput(dummyInfo);
        return true;
    }

    void OutputConsoleMonitor::handleOutput(const OutputPacket &outputInfo) {
        static Vector<Real> measures;
        auto elTime = timer.getElTime_sec();

        auto n = params.getn();
        auto currn = outputInfo.getSteps();
        static auto lastn = currn;

        auto t = outputInfo.getSimTime();
        Log::Info() << Log::Flush;
        Log::Info() << (100 * t / maxT) << "% done" << Log::Flush;
        Log::Info() << "Step " << outputInfo.getSteps() << "/" << n << Log::Flush;
        Log::Info() << "t = " << t << "/" << maxT << Log::Flush;

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

            Log::Info() << "Avg " << stepsPerSec << " steps/s in last " << total << " measures" << Log::Flush;
            Log::Info() << "El time since last step: " << elTime << "s" << Log::Flush;
            Log::Info() << "Expected finish in " << Log::FGBlue << int(expectedFinish) / 60 << "m "
                        << int(expectedFinish) % 60 << "s" << Log::Flush;

            // this->setnSteps(stepsPerSec);
        }

        lastn = currn;

        timer.reset();
    }


}