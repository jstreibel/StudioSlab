//
// Created by joao on 8/23/22.
//

#include "Histogram.h"
#include "Core/Tools/Log.h"

#define bin(v) int( round ( (v-vMin)/binWidth ) )


namespace Slab::Math::RtoR {

    Histogram::Histogram() {

    }

    void Histogram::Compute(const RealArray &F, int nbins) {
        this->nBins = nbins ;

        auto max = F[0];
        auto min = F[0];
        for (auto &v: F) {
            if (v > max) max = v;
            else if (v < min) min = v;
        }

        if (max == min) max = min + 0.1;

        vMax = max;
        vMin = min;

        if (vMax == vMin) return;

        binWidth = (vMax - vMin) / Real(nBins - 1);

        bins.clear();
        bins.resize(nBins, 0);
        count = F.size();

        for (auto &v: F)
            bins[bin(v)]++;
    }

    void Histogram::Compute(RtoR::NumericFunction_constptr func, int _nBins) {
        auto &F = func->getSpace().getHostData();

        Compute(F, _nBins);
    }

    RtoR::Function *Histogram::asPDFFunction() const {
        auto *func = new RtoR::NumericFunction_CPU(bins.size(), vMin, vMax);

        auto &F = func->getSpace().getHostData();

        assert(F.size() == bins.size() && bins.size() == nBins);

        auto N = Real(count);
        auto normFactor = N * binWidth;

        for (auto i = 0; i < nBins; ++i)
            F[i] = bins[i] / normFactor;

        return func;
    }

    auto Histogram::asPDFPointSet(bool beautiful) const -> PointSet_ptr {
        PointSet_ptr pointSet;
        return renderPDFToPointSet(pointSet, beautiful);
    }

    auto Histogram::renderPDFToPointSet(PointSet_ptr pointSet, bool beautiful) const -> PointSet_ptr {
        if(pointSet == nullptr) pointSet = Slab::New<PointSet>();

        const auto N = Real(count);
        const auto w = binWidth;
        const auto normFactor = N * w;

        // auto &points = pointSet->getPoints();
        // points.clear();
        pointSet->clear();

        if (beautiful) pointSet->addPoint({vMin, 0});

        for (auto i = 0; i < nBins; ++i) {
            auto nHere = bins[i];

            auto x = vMin + i * w;
            auto y = (Real) nHere / normFactor;

            pointSet->addPoint({x, y});
            if (beautiful) {
                pointSet->addPoint({x + w, y});
                pointSet->addPoint({x + w, 0});
            }
        }

        return pointSet;
    }

    auto Histogram::integrate() const -> Real {
        let sum = .0;

        for (fix density: bins)
            sum += density;

        return sum / (double) count;
    }

}