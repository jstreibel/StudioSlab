//
// Created by joao on 8/23/22.
//

#include "Histogram.h"
#include "Phys/Numerics/Allocator.h"

#define bin(v) int(floor(((v)-xMin)/binWidth))

Histogram::Histogram() {

}

void Histogram::Compute(const RtoR::ArbitraryFunction &func, int nBins) {

    this->nBins = nBins;

    auto &F = func.getSpace().getX();

    auto max = F[0];
    auto min = F[0];
    for(auto &v : F){
        if(v > max) max = v;
        else if(v < min) min = v;
    }

    if(max == min) max = min+0.1;

    xMax = max;
    xMin = min;

    if(xMax == xMin) return;

    binWidth = (xMax - xMin) / Real(nBins);

    bins.clear();
    bins.resize(nBins, 0);
    count=F.size();

    for(auto &v : F) {
        bins[bin(v)]++;
    }

}

RtoR::Function *Histogram::asPDFFunction() const {
    auto *func = new RtoR::FunctionArbitraryCPU(bins.size(), xMin, xMax);

    auto &F = func->getSpace().getX();

    assert(F.size() == bins.size() && bins.size() == nBins);

    auto N=Real(count);
    auto normFactor = N*binWidth;

    for(auto i=0; i<nBins; ++i)
        F[i] = bins[i]/normFactor;

    return func;
}


