//
// Created by joao on 15/05/2021.
//

#ifndef ISING_RANDUTILS_H
#define ISING_RANDUTILS_H

#include "Utils/Types.h"

namespace Slab {

    namespace RandUtils {
        void seed(int s);

        Real random01();

        Real random(Real a, Real b);

        unsigned RandInt();

        double GaussianNoise(Real mean = 0.0, Real standard_deviation = 1.0, bool reSeed = false, int seed = 1);
    }

    namespace LostAndFound {
        void imprimeBinario(unsigned long long a);
    }

}

#endif //ISING_RANDUTILS_H
