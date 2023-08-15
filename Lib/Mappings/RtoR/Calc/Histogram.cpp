//
// Created by joao on 8/23/22.
//

#include "Histogram.h"

#define bin(v) int(floor(((v)-vMin)/binWidth))

Histogram::Histogram() {

}

void Histogram::Compute(const RtoR::DiscreteFunction &func, int nBins) {

    this->nBins = nBins;

    auto &F = func.getSpace().getHostData();

    auto max = F[0];
    auto min = F[0];
    for(auto &v : F){
        if(v > max) max = v;
        else if(v < min) min = v;
    }

    if(max == min) max = min+0.1;

    vMax = max;
    vMin = min;

    if(vMax == vMin) return;

    binWidth = (vMax - vMin) / Real(nBins);

    bins.clear();
    bins.resize(nBins, 0);
    count=F.size();

    for(auto &v : F) {
        bins[bin(v)]++;
    }

}

RtoR::Function *Histogram::asPDFFunction() const {
    auto *func = new RtoR::FunctionArbitraryCPU(bins.size(), vMin, vMax);

    auto &F = func->getSpace().getHostData();

    assert(F.size() == bins.size() && bins.size() == nBins);

    auto N=Real(count);
    auto normFactor = N*binWidth;

    for(auto i=0; i<nBins; ++i)
        F[i] = bins[i]/normFactor;

    return func;
}

auto Histogram::asPointSet() const -> Spaces::PointSet {
    Spaces::PointSet pointSet;
    return renderToPointSet(pointSet);
}

auto Histogram::renderToPointSet(Spaces::PointSet &pointSet) const -> Spaces::PointSet {
    const auto N=Real(count);
    const auto w = binWidth;
    const auto normFactor = N*w;

    Spaces::Point2DVec points;

    points.push_back({vMin, 0});

    for(auto i=0; i<nBins; ++i) {
        auto x = vMin + i*w;
        auto y = bins[i] / normFactor;

        points.push_back({x, y});
        points.push_back({x+w, y});
        points.push_back({x+w, 0});
    }

    return pointSet = Spaces::PointSet(points);
}


