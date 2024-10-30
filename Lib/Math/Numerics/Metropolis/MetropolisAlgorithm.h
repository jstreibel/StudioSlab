//
// Created by joao on 10/28/24.
//

#ifndef STUDIOSLAB_METROPOLISALGORITHM_H
#define STUDIOSLAB_METROPOLISALGORITHM_H

#include "Utils/Numbers.h"

namespace Slab::Math {

    using Temperature = Real;

    class MetropolisAlgorithm {
        Temperature T=0;

    public:
        explicit MetropolisAlgorithm(Temperature);

        virtual void step() = 0;
    };



} // Slab::Math

#endif //STUDIOSLAB_METROPOLISALGORITHM_H
