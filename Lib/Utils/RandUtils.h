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

        Real RandomUniformReal01();
        Real RandomUniformReal(Real a, Real b);
        unsigned RandomUniformUInt();

        double GaussianNoise(double mean, double standard_deviation);

        std::vector<double> GenerateLognormalValues(int n, double x0, double x_std, int seed);

    }

    namespace LostAndFound {
        void imprimeBinario(unsigned long long a);
    }

}

#endif //ISING_RANDUTILS_H
