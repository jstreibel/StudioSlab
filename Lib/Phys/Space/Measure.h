//
// Created by joao on 11/4/21.
//

#ifndef STUDIOSLAB_MEASURE_H
#define STUDIOSLAB_MEASURE_H


#include "Common/CoreNativeTypes.h"

class Measure {
    std::vector<Real> linearValues;

public:
    Measure(std::vector<Real> linearValues);

    Real operator*();
};


#endif //STUDIOSLAB_MEASURE_H
