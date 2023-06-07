//
// Created by joao on 30/09/2019.
//

#include "BenchmarkHistogram.h"

#include "Common/Types.h"

#include <cmath>
#include <cassert>


// #define nClasses (1 + (lastClass-firstClass) / classInterval) // +1 pra sobras
#define classInterval ((lastClass-firstClass) / nClasses)

BenchmarkHistogram::BenchmarkHistogram(int firstClass, int lastClass, int nClasses)
: C0(firstClass), VLast(lastClass+classInterval-1), I(classInterval), invI(1./classInterval), histogram(nClasses) {
    std::cout << "Benchmark histogram started with " << histogram.size() << " classes "
                                                                       "of width " << classInterval << " each." << std::endl;
}

BenchmarkHistogram::~BenchmarkHistogram() {

}

void BenchmarkHistogram::startMeasure() {
    timer = CPUTimer();
}

void BenchmarkHistogram::storeMeasure(int nSteps) {
    if(firstMeasure) { firstMeasure=false; return; }

    timer.stop();

    const auto measure = timer.elapsed().wall/nSteps;

    measures.push_back(measure);

    int V = measure/1000; // ns -> \mu s

    if(V<C0 || V>VLast) return;
    int i = int(floorf((V-C0)*invI));
    histogram[i]++;
    count++;
}

auto BenchmarkHistogram::getAverage() const -> boost::timer::nanosecond_type {
    if(count != measures.size()) std::cout << "Benchmark inconsistency: " << count << " != " << measures.size();

    boost::timer::nanosecond_type sum=0.;
    if(0) for (size_t i=0; i<histogram.size(); i++) {
        const auto C=Real(i*I+C0);
        sum += C*histogram[i];
    }
    else for(auto v : measures) sum += v;

    return count > 0 ? sum / count : 0;
}

void BenchmarkHistogram::printHistogram(std::ostream &out) const {
    if(0) {
        int C = C0;
        for (auto val : histogram) {
            out << std::endl << C << " - " << (C += I) << ": ";
            for (int i = 0; i < val; i += 1)
                out << char(219);
        }
    } else {
        const auto avg = long(getAverage()*1e-3);

        std::cout << "\nHistogram measured average time: " << avg << " μs/step" << std::endl;
    }
}

std::ostream &operator<<(std::ostream &os, const BenchmarkHistogram &hist) {

    hist.printHistogram(os);

    return os;
}

