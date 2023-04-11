//
// Created by joao on 30/09/2019.
//

#ifndef V_SHAPE_BENCHMARKHISTOGRAM_H
#define V_SHAPE_BENCHMARKHISTOGRAM_H

//#include "../Util/Workaround/ABIDef.h"

#include <boost/timer/timer.hpp>
#include <vector>
#include <iostream>


typedef boost::timer::cpu_timer CPUTimer;
typedef boost::timer::cpu_times CPUTimes;

class BenchmarkHistogram {
public:
    explicit BenchmarkHistogram(int firstClass=100, int lastClass=150100, int nClasses=100);
    ~BenchmarkHistogram();
    void startMeasure();
    void storeMeasure(int nSteps=1);

    auto getAverage() const -> boost::timer::nanosecond_type;

    void printHistogram(std::ostream &out = std::cout) const;
    friend std::ostream& operator<<(std::ostream& os, const BenchmarkHistogram& hist);

private:
    bool firstMeasure = true;

    CPUTimer timer;

    int count=0;
    const int C0,I,VLast;
    const double invI;
    std::vector<int> histogram;
    std::vector<boost::timer::nanosecond_type> measures;
};


#endif //V_SHAPE_BENCHMARKHISTOGRAM_H
