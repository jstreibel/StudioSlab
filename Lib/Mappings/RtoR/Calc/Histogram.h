//
// Created by joao on 8/23/22.
//

#ifndef STUDIOSLAB_HISTOGRAM_H
#define STUDIOSLAB_HISTOGRAM_H

#include "Mappings/RtoR/Model/RtoRFunctionArbitrary.h"
#include "Mappings/RtoR/Model/RtoRFunctionArbitraryCPU.h"

class Histogram {
public:
    Histogram();

    void Compute(const RtoR::ArbitraryFunction &func, int nBins = 100);

    RtoR::Function* asPDFFunction() const;

    int nBins;
    Real xMax, xMin;
    Real binWidth;

private:
    unsigned long count;
    VecFloat bins;

};


#endif //STUDIOSLAB_HISTOGRAM_H
