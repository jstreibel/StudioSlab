//
// Created by joao on 11/4/21.
//

#ifndef STUDIOSLAB_MEASURE_H
#define STUDIOSLAB_MEASURE_H


#include "Utils/Types.h"

namespace Slab::Math {

    class Measure {
        RealVector linearValues;

    public:
        Measure(RealVector linearValues);

        Real operator*();
    };

}

#endif //STUDIOSLAB_MEASURE_H
