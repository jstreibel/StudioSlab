//
// Created by joao on 3/27/23.
//

#include "BinaryToInt.h"

#include <bitset>
#include <iostream>
#include <algorithm>

std::string replace(std::string str, char oldChar, char newChar) {
    std::replace(str.begin(), str.end(), oldChar, newChar);

    return str;
}

unsigned BinaryToUInt(std::string binary, char zero, char one) {
    if(zero != '0') binary = replace(binary, zero, '0');
    if(one  != '1') binary = replace(binary, one,  '1');

    std::cout << binary << std::endl;

    std::bitset<32>  x(binary);

    return x.to_ullong();
}

unsigned short BinaryToUShort(std::string binary, char zero, char one){
    if(zero != '0') binary = replace(binary, zero, '0');
    if(one  != '1') binary = replace(binary, one,  '1');

    std::bitset<16>  x(binary);

    return x.to_ulong();
}