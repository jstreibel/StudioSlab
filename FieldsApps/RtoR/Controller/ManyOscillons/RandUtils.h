//
// Created by joao on 15/05/2021.
//

#ifndef ISING_RANDUTILS_H
#define ISING_RANDUTILS_H

namespace RandUtils {
    auto random01() -> double;
    auto random(double a, double b) -> double;

    const int RandInt();
}

namespace LostAndFound {
    void imprimeBinario(unsigned long long a);
}

#endif //ISING_RANDUTILS_H
