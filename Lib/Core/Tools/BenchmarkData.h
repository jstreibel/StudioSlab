//
// Created by joao on 30/09/2019.
//

#ifndef V_SHAPE_BENCHMARKHISTOGRAM_H
#define V_SHAPE_BENCHMARKHISTOGRAM_H

//#include "../Util/Workaround/ABIDef.h"

#include "Utils/Types.h"
#include "Utils/Timer.h"
#include "Utils/List.h"
#include "Log.h"


namespace Slab::Core {

    class BenchmarkData {
        CPUTimer Timer;
        Vector<Nanosecond> Measures;

    public:
        explicit BenchmarkData(CountType reserveMemory=0);

        ~BenchmarkData();

        void StartMeasure();

        void StoreMeasure(int nSteps = 1);

        auto GetAverage() const -> boost::timer::nanosecond_type;

        void Print(OStream &out = Log::Info()) const;

        friend OStream &operator<<(OStream &os, const BenchmarkData &hist);

    };

}

#endif //V_SHAPE_BENCHMARKHISTOGRAM_H
