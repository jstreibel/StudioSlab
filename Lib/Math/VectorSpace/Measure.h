//
// Created by joao on 11/4/21.
//

#ifndef STUDIOSLAB_MEASURE_H
#define STUDIOSLAB_MEASURE_H


#include "Utils/Types.h"

namespace Slab::Math {

    class Measure {
        FRealVector linearValues;

    public:
        Measure(FRealVector linearValues);

        DevFloat operator*();
    };

}

#endif //STUDIOSLAB_MEASURE_H
