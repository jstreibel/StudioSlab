//
// Created by joao on 8/23/22.
//

#ifndef STUDIOSLAB_HISTOGRAM_H
#define STUDIOSLAB_HISTOGRAM_H

#include "Mappings/RtoR/Model/RtoRDiscreteFunction.h"
#include "Mappings/RtoR/Model/RtoRDiscreteFunctionCPU.h"

class Histogram {
public:
    Histogram();

    void Compute(const RtoR::DiscreteFunction &func, int nBins = 100);

    RtoR::Function* asPDFFunction() const;

    auto asPDFPointSet(bool beautiful=true) const -> Spaces::PointSet;
    auto renderPDFToPointSet(Spaces::PointSet &pointSet, bool beautiful=true) const -> Spaces::PointSet;

    auto integrate() const -> Real;

    int nBins;
    Real vMax, vMin;
    Real binWidth;

private:
    unsigned long count;
    RealVector bins;

};


#endif //STUDIOSLAB_HISTOGRAM_H
