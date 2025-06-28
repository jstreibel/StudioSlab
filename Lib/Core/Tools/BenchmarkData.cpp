//
// Created by joao on 30/09/2019.
//

#include "BenchmarkData.h"

#include "Utils/Types.h"
#include "Core/Tools/Log.h"

#include <cmath>
#include <cassert>



namespace Slab::Core {


// #define nClasses (1 + (lastClass-firstClass) / classInterval) // +1 pra sobras
#define classInterval ((lastClass-firstClass) / nClasses)

    BenchmarkData::BenchmarkData(CountType reserveMemory) {
        Measures.reserve(reserveMemory);
        Log::Status() << "BenchmarkData started." << Log::Flush;
    }

    BenchmarkData::~BenchmarkData() = default;

    void BenchmarkData::StartMeasure() { Timer = CPUTimer(); }

    void BenchmarkData::StoreMeasure(int nSteps) {
        Timer.stop();

        const auto measure = Timer.elapsed().wall / nSteps;

        Measures.push_back(measure);

        /*
        int i = int(floorf((V - C0) * invI));
        histogram[i]++;
         */
    }

    auto BenchmarkData::GetAverage() const -> Nanosecond {
        Nanosecond sum = 0;

        for (auto v: Measures) sum += v;

        fix count = Measures.size();

        return count > 0 ? sum / (Nanosecond)count : 0;
    }

    void BenchmarkData::Print(OStream &out) const {
            const auto avg = long((DevFloat)GetAverage() * 1e-3);

            out << "Histogram measured average time: " << avg << " μs/step.";
    }

    OStream &operator<<(OStream &os, const BenchmarkData &hist) {

        hist.Print(os);

        return os;
    }

}