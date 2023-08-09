//
// Created by joao on 15/05/2021.
//

#ifndef ISING_RANDUTILS_H
#define ISING_RANDUTILS_H

#include "Common/Types.h"

namespace RandUtils {
    Real random01();

    Real random(Real a, Real b);

    int RandInt();
}

namespace LostAndFound {
    void imprimeBinario(unsigned long long a);
}

#endif //ISING_RANDUTILS_H
