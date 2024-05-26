//
// Created by joao on 10/25/21.
//

#ifndef STUDIOSLAB_TYPES_H
#define STUDIOSLAB_TYPES_H

#include "DeviceConfig.h"
#include "NativeTypes.h"

#include "Numbers.h"
#include "Arrays.h"

#include "Pointer.h"

#include "Exception.h"


namespace Slab {

#define let     auto
#define OUT auto&
#define IN  const auto&
#define GET auto&
#define fix     const auto
#define look    const auto *

    template <typename T1, typename T2>
    using Pair = std::pair<T1, T2>;

    typedef Pair<int, int> IntPair;

    namespace Math {
        typedef Pair<const class DiscreteSpace *, const class DiscreteSpace *> DiscreteSpacePair;
    }

}


#endif //STUDIOSLAB_TYPES_H
