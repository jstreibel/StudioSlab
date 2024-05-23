//
// Created by joao on 23/09/23.
//

#ifndef STUDIOSLAB_NUMBERS_NATIVE_TYPES_H
#define STUDIOSLAB_NUMBERS_NATIVE_TYPES_H

#include "NativeTypes.h"

#include <complex>

namespace Slab {

    const auto HUGE_NUMBER = ((unsigned long long) 0) - 1;

    typedef floatt Real;
    typedef float Real32;
    typedef double Real64;
    typedef std::complex<Real> Complex;

    typedef unsigned char Byte;
    typedef unsigned char *ByteData_raw;
    typedef char *RawData;
    typedef Real32 *RealData;
    typedef int Int;
    typedef unsigned int UInt;
    typedef long long IntBig;
    typedef unsigned long UIntBig;
    typedef std::size_t Count;
    typedef std::size_t Resolution;
    typedef int_least64_t BigInt;

    union DoubleAccess {
        double val;
        char bytes[8];

        inline bool isPositive() const {
            return !((bytes[7] & 0x80) >> 7);

        }
    };

}

#endif //STUDIOSLAB_NUMBERS_NATIVE_TYPES_H
