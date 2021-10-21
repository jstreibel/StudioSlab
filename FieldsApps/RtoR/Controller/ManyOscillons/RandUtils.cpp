//
// Created by joao on 15/05/2021.
//

#include "RandUtils.h"

#include <iostream>
#include <random>

namespace RandUtils {
    std::mt19937 mt(1);
    std::uniform_real_distribution<double> randGen(0, 1);

    double random01() {
        return randGen(mt);
    }

    double random(double a, double b) {
        return a + random01()*(b-a);
    }

    const int RandInt() {
        return rand();
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