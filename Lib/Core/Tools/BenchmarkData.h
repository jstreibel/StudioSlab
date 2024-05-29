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
        CPUTimer timer;
        Vector<Nanosecond> measures;

    public:
        explicit BenchmarkData(Count reserveMemory=0);

        ~BenchmarkData();

        void startMeasure();

        void storeMeasure(int nSteps = 1);

        auto getAverage() const -> boost::timer::nanosecond_type;

        void print(OStream &out = Log::Info()) const;

        friend OStream &operator<<(OStream &os, const BenchmarkData &hist);

    private:

    };

}

#endif //V_SHAPE_BENCHMARKHISTOGRAM_H
