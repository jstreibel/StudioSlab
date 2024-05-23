//
// Created by joao on 8/23/22.
//

#include "Histogram.h"
#include "Core/Tools/Log.h"

#define bin(v) int( round ( (v-vMin)/binWidth ) )


namespace Slab::Math::RtoR {

    Histogram::Histogram() {

    }

    void Histogram::Compute(RtoR::DiscreteFunction_constptr func, int _nBins) {

        this->nBins = _nBins;

        auto &F = func->getSpace().getHostData();

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

    RtoR::Function *Histogram::asPDFFunction() const {
        auto *func = new RtoR::DiscreteFunction_CPU(bins.size(), vMin, vMax);

        auto &F = func->getSpace().getHostData();

        assert(F.size() == bins.size() && bins.size() == nBins);

        auto N = Real(count);
        auto normFactor = N * binWidth;

        for (auto i = 0; i < nBins; ++i)
            F[i] = bins[i] / normFactor;

        return func;
    }

    auto Histogram::asPDFPointSet(bool beautiful) const -> Math::PointSet {
        Math::PointSet pointSet;
        return renderPDFToPointSet(pointSet, beautiful);
    }

    auto Histogram::renderPDFToPointSet(Math::PointSet &pointSet, bool beautiful) const -> Math::PointSet {
        const auto N = Real(count);
        const auto w = binWidth;
        const auto normFactor = N * w;

        Math::Point2DVec points;

        if (beautiful) points.emplace_back(vMin, 0);

        static Count debugCount;
        static Count debugCount_last = 0;
        debugCount = 0;

        for (auto i = 0; i < nBins; ++i) {
            auto nHere = bins[i];

            auto x = vMin + i * w;
            auto y = (Real) nHere / normFactor;

            points.emplace_back(x, y);
            if (beautiful) {
                points.emplace_back(x + w, y);
                points.emplace_back(x + w, 0);
            }

            debugCount += nHere;
        }

        if (debugCount != debugCount_last && debugCount_last != 0)
            Core::Log::Info("Histogram counting error: ") << debugCount << Core::Log::Flush;

        debugCount_last = debugCount;

        return pointSet = Math::PointSet(points);
    }

    auto Histogram::integrate() const -> Real {
        let sum = .0;

        for (fix density: bins)
            sum += density;

        return sum / (double) count;
    }


}