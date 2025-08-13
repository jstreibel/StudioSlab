//
// Created by joao on 15/05/2021.
//

#ifndef ISING_RANDUTILS_H
#define ISING_RANDUTILS_H

#include "Utils/Types.h"

namespace Slab {

    namespace RandUtils {
        void SeedUniformReal  (int s);
        void SeedUniformUInt  (int s);
        void SeedGaussianNoise(int s);

        DevFloat RandomUniformReal01();
        DevFloat RandomUniformReal(DevFloat a, DevFloat b);
        unsigned RandomUniformUInt();

        double GaussianNoise(double mean, double standard_deviation);

        std::vector<double> GenerateLognormalValues(int n, double x0, double x_std, int seed);

    }

    namespace LostAndFound {
        void imprimeBinario(unsigned long long a);
    }

}

#endif //ISING_RANDUTILS_H
