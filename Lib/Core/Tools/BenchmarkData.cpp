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

    BenchmarkData::BenchmarkData(Count reserveMemory) {
        measures.reserve(reserveMemory);
        Log::Status() << "BenchmarkData started." << Log::Flush;
    }

    BenchmarkData::~BenchmarkData() = default;

    void BenchmarkData::startMeasure() { timer = CPUTimer(); }

    void BenchmarkData::storeMeasure(int nSteps) {
        timer.stop();

        const auto measure = timer.elapsed().wall / nSteps;

        measures.push_back(measure);

        /*
        int i = int(floorf((V - C0) * invI));
        histogram[i]++;
         */
    }

    auto BenchmarkData::getAverage() const -> Nanosecond {
        Nanosecond sum = 0;

        for (auto v: measures) sum += v;

        fix count = measures.size();

        return count > 0 ? sum / (Nanosecond)count : 0;
    }

    void BenchmarkData::print(OStream &out) const {
            const auto avg = long((Real)getAverage() * 1e-3);

            out << "Histogram measured average time: " << avg << " μs/step.";
    }

    OStream &operator<<(OStream &os, const BenchmarkData &hist) {

        hist.print(os);

        return os;
    }

}