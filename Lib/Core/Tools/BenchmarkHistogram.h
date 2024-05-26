//
// Created by joao on 30/09/2019.
//

#ifndef V_SHAPE_BENCHMARKHISTOGRAM_H
#define V_SHAPE_BENCHMARKHISTOGRAM_H

//#include "../Util/Workaround/ABIDef.h"

#include "Utils/Types.h"

#include <boost/timer/timer.hpp>
#include <vector>
#include <iostream>


namespace Slab::Core {

    typedef boost::timer::cpu_timer CPUTimer;
    typedef boost::timer::cpu_times CPUTimes;

    class BenchmarkHistogram {
    public:
        explicit BenchmarkHistogram(int firstClass = 100, int lastClass = 150100, int nClasses = 100);

        ~BenchmarkHistogram();

        void startMeasure();

        void storeMeasure(int nSteps = 1);

        auto getAverage() const -> boost::timer::nanosecond_type;

        void printHistogram(OStream &out = std::cout) const;

        friend OStream &operator<<(OStream &os, const BenchmarkHistogram &hist);

    private:
        bool firstMeasure = true;

        CPUTimer timer;

        int count = 0;
        const int C0, I, VLast;
        const Real invI;
        Vector<int> histogram;
        Vector<boost::timer::nanosecond_type> measures;
    };

}

#endif //V_SHAPE_BENCHMARKHISTOGRAM_H
