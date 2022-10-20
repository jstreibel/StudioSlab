//
// Created by joao on 10/25/21.
//

#ifndef STUDIOSLAB_TYPES_H
#define STUDIOSLAB_TYPES_H

#include "DeviceConfig.h"
#include "STDLibInclude.h"

typedef size_t PosInt;
typedef int_least64_t BigInt;

template<class T1, class T2>
struct Pair {
    T1 first;
    T2 second;
};

typedef Pair<int, int> IntPair;
typedef std::pair<const class DiscreteSpace*, const class DiscreteSpace*> DiscreteSpacePair;

#endif //STUDIOSLAB_TYPES_H
