//
// Created by joao on 30/09/2019.
//

#ifndef V_SHAPE_BENCHMARKHISTOGRAM_H
#define V_SHAPE_BENCHMARKHISTOGRAM_H

#include "Studios/Util/Workaround/ABIDef.h"

#include <boost/timer/timer.hpp>
#include <vector>

#include "MathTypes.h"

typedef boost::timer::cpu_timer CPUTimer;
typedef boost::timer::cpu_times CPUTimes;

class BenchmarkHistogram {
public:
    explicit BenchmarkHistogram(BigInt firstClass=100, BigInt lastClass=150100, BigInt nClasses=10000);
    ~BenchmarkHistogram();
    void startMeasure();
    void storeMeasure(PosInt nSteps=1);

    auto getAverage() const -> double;

    void printHistogram(std::ostream &out = std::cout) const;
    friend std::ostream& operator<<(std::ostream& os, const BenchmarkHistogram& hist);

private:
    CPUTimer timer;

    BigInt count=0;
    const BigInt C0,I,VLast;
    const double invI;
    std::vector<BigInt> histogram;
};


#endif //V_SHAPE_BENCHMARKHISTOGRAM_H
