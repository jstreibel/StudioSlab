//
// Created by joao on 30/09/2019.
//

#include "BenchmarkHistogram.h"

#include <cmath>


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
    timer.stop();

    int V = timer.elapsed().wall/(1000*nSteps); // ns -> \mu s

    if(V<C0 || V>VLast) return;
    int i = int(floorf((V-C0)*invI));
    histogram[i]++;
    count++;
}

auto BenchmarkHistogram::getAverage() const -> double {
    double sum=0.;
    for(size_t i=0; i<histogram.size(); i++){
        const auto C=double(i*I+C0);
        sum += C*histogram[i];
    }

    const double avg = sum / count;

    return avg;
}

void BenchmarkHistogram::printHistogram(std::ostream &out) const {
    if(1) {
        int C = C0;
        for (auto val : histogram) {
            out << std::endl << C << " - " << (C += I) << ": ";
            for (int i = 0; i < val; i += 1)
                out << char(219);
        }
    } else {
        const double avg = getAverage();

        std::cout << "Average time is " << avg << " microseconds/step" << std::endl;
    }
}

std::ostream &operator<<(std::ostream &os, const BenchmarkHistogram &hist) {

    hist.printHistogram(os);

    return os;
}

