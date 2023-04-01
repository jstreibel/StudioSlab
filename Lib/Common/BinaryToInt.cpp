//
// Created by joao on 3/27/23.
//

#include "BinaryToInt.h"

unsigned long long binaryToUInt(std::string binary) {
    std::bitset<32>  x(binary);

    return x.to_ullong();
}