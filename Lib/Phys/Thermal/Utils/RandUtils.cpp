//
// Created by joao on 15/05/2021.
//

#include "RandUtils.h"

#include <iostream>
#include <random>

namespace RandUtils {
    std::mt19937 mt(2);
    std::uniform_real_distribution<double> realRandGen(0, 1);

    std::uniform_int_distribution<unsigned int> uintRandGen(0, INT32_MAX);

    double random01() {
        return realRandGen(mt);
    }

    double random(double a, double b){
        return (b-a)*random01() + a;
    }

    int RandInt() {
        return uintRandGen(mt);
    }
}


namespace LostAndFound {
    void imprimeBinario(uint64_t a) {
        for (int i = 63; i >= 0; --i) {
            int val = (a & (1 << i)) >> i;
            std::cout << val;
        }
    }
}