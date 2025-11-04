//
// Created by joao on 8/23/22.
//

#include "Histogram.h"
#include "Core/Tools/Log.h"
#include "Math/Data/DataAllocator.h"

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

        binWidth = (vMax - vMin) / DevFloat(nBins - 1);

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

    TPointer<RtoR::Function> Histogram::asPDFFunction() const {
        auto func = DataAlloc<RtoR::NumericFunction_CPU>("", bins.size(), vMin, vMax);

        auto &F = func->getSpace().getHostData();

        assert(F.size() == bins.size() && bins.size() == nBins);

        auto N = DevFloat(count);
        auto normFactor = N * binWidth;

        for (auto i = 0; i < nBins; ++i)
            F[i] = bins[i] / normFactor;

        return func;
    }

    auto Histogram::asPDFPointSet(bool beautiful) const -> FPointSet_ptr {
        FPointSet_ptr pointSet;
        return renderPDFToPointSet(pointSet, beautiful);
    }

    auto Histogram::renderPDFToPointSet(FPointSet_ptr pointSet, bool beautiful) const -> FPointSet_ptr {
        if(pointSet == nullptr) pointSet = Slab::New<FPointSet>();

        const auto N = DevFloat(count);
        const auto w = binWidth;
        const auto normFactor = N * w;

        // auto &points = pointSet->getPoints();
        // points.clear();
        pointSet->clear();

        if (beautiful) pointSet->AddPoint({vMin, 0});

        for (auto i = 0; i < nBins; ++i) {
            auto nHere = bins[i];

            auto x = vMin + i * w;
            auto y = (DevFloat) nHere / normFactor;

            pointSet->AddPoint({x, y});
            if (beautiful) {
                pointSet->AddPoint({x + w, y});
                pointSet->AddPoint({x + w, 0});
            }
        }

        return pointSet;
    }

    auto Histogram::integrate() const -> DevFloat {
        let sum = .0;

        for (fix density: bins)
            sum += density;

        return sum / (double) count;
    }

}