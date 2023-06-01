//
// Created by joao on 11/4/21.
//

#ifndef STUDIOSLAB_MEASURE_H
#define STUDIOSLAB_MEASURE_H


#include "Common/Types.h"

class Measure {
    VecFloat linearValues;

public:
    Measure(VecFloat linearValues);

    Real operator*();
};


#endif //STUDIOSLAB_MEASURE_H
