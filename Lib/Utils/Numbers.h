//
// Created by joao on 23/09/23.
//

#ifndef STUDIOSLAB_NUMBERS_NATIVE_TYPES_H
#define STUDIOSLAB_NUMBERS_NATIVE_TYPES_H

#include "NativeTypes.h"

#include <complex>
// #include <quadmath.h>

namespace Slab {

    const auto HUGE_NUMBER = ((unsigned long long) 0) - 1;

    typedef floatt DevFloat;
    typedef float Real32;
    typedef double Real64;
    typedef __float128 Real128;
    typedef std::complex<DevFloat> Complex;

    typedef unsigned char Byte;
    typedef unsigned char *ByteData_raw;
    typedef char *RawData;
    typedef Real32 *RealData;
    typedef int Int;
    typedef unsigned int UInt;
    typedef long long IntBig;
    typedef unsigned long UIntBig;
    typedef std::size_t CountType;
    typedef std::size_t Index;
    typedef std::size_t Resolution;
    constexpr Resolution UndefinedResolution = (unsigned)-1;
    typedef int_least64_t BigInt;

    const DevFloat Infinity = std::numeric_limits<double>::infinity();

    template<typename T>
    inline T Max(const T&a, const T&b) {
        return std::max(a, b);
    }

    template<typename T>
    inline T Min(const T&a, const T&b) {
        return std::min(a, b);
    }

    union DoubleAccess {
        double val;
        char bytes[8];

        inline bool isPositive() const {
            return !((bytes[7] & 0x80) >> 7);

        }
    };

}

#endif //STUDIOSLAB_NUMBERS_NATIVE_TYPES_H
