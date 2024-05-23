//
// Created by joao on 8/23/22.
//

#ifndef STUDIOSLAB_HISTOGRAM_H
#define STUDIOSLAB_HISTOGRAM_H

#include "Math/Function/RtoR/Model/RtoRDiscreteFunction.h"
#include "Math/Function/RtoR/Model/RtoRDiscreteFunctionCPU.h"


namespace Slab::Math::RtoR {

    class Histogram {
    public:
        Histogram();

        void Compute(RtoR::DiscreteFunction_constptr func, int nBins = 100);

        RtoR::Function *asPDFFunction() const;

        auto asPDFPointSet(bool beautiful = true) const -> Math::PointSet;

        auto renderPDFToPointSet(Math::PointSet &pointSet, bool beautiful = true) const -> Math::PointSet;

        auto integrate() const -> Real;

        int nBins;
        Real vMax, vMin;
        Real binWidth;

    private:
        unsigned long count;
        RealVector bins;

    };

}

#endif //STUDIOSLAB_HISTOGRAM_H
