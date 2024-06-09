//
// Created by joao on 8/23/22.
//

#ifndef STUDIOSLAB_HISTOGRAM_H
#define STUDIOSLAB_HISTOGRAM_H

#include "Math/Function/RtoR/Model/RtoRNumericFunction.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"


namespace Slab::Math::RtoR {

    class Histogram {
    public:
        Histogram();

        void Compute(RtoR::NumericFunction_constptr func, int nBins = 100);

        RtoR::Function *asPDFFunction() const;

        auto asPDFPointSet(bool beautiful = true) const -> PointSet_ptr;

        auto renderPDFToPointSet(PointSet_ptr out, bool beautiful = true) const -> PointSet_ptr;

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
