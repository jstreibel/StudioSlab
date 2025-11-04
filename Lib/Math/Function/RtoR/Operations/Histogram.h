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

        void Compute(const RealArray &, int nbins=100);
        void Compute(RtoR::NumericFunction_constptr func, int nBins = 100);

        TPointer<RtoR::Function> asPDFFunction() const;

        auto asPDFPointSet(bool beautiful = true) const -> FPointSet_ptr;

        auto renderPDFToPointSet(FPointSet_ptr out, bool beautiful = true) const -> FPointSet_ptr;

        auto integrate() const -> DevFloat;

        int nBins;
        DevFloat vMax, vMin;
        DevFloat binWidth;

    private:
        unsigned long count;
        FRealVector bins;

    };

}

#endif //STUDIOSLAB_HISTOGRAM_H
