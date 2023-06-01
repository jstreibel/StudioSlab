//
// Created by joao on 13/09/2019.
//

#include "Utils.h"

void Common::PrintThere(int x, int y, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    printf("\033[%d;%dH", x, y);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
}

char Common::GetDensityChar(float dens){
    const int N = 70;
    const char* ascii_gs = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`\'. ";

    int loc = (N-dens*N);
    loc = loc>N-1?N-1:loc<0?0:loc;

    return ascii_gs[loc];
}

void Common::PrintDensityThere(int x, int y, float dens) {
    PrintThere(x, y, "%c", GetDensityChar(dens));
}

StrVector Common::splitString(const String &input, char delimiter) {
    StrVector tokens;
    std::size_t start = 0;
    std::size_t end = input.find(delimiter);

    while (end != String::npos) {
        std::string token = input.substr(start, end - start);
        tokens.push_back(token);
        start = end + 1;
        end = input.find(delimiter, start);
    }

    // Add the last token (or the only token if no delimiter is found)
    std::string lastToken = input.substr(start);
    tokens.push_back(lastToken);

    return tokens;
}

#include <bitset>
#include <iostream>
#include <algorithm>

std::string replace(std::string str, char oldChar, char newChar) {
    std::replace(str.begin(), str.end(), oldChar, newChar);

    return str;
}

unsigned Common::BinaryToUInt(std::string binary, char zero, char one) {
    if(zero != '0') binary = replace(binary, zero, '0');
    if(one  != '1') binary = replace(binary, one,  '1');

    std::bitset<32>  x(binary);

    return x.to_ullong();
}

unsigned short Common::BinaryToUShort(std::string binary, char zero, char one){
    if(zero != '0') binary = replace(binary, zero, '0');
    if(one  != '1') binary = replace(binary, one,  '1');

    std::bitset<16>  x(binary);

    return x.to_ulong();
}

#if USE_CUDA

#include <cuda_runtime.h>
void cew(cudaError err){

    auto errStr = cudaGetErrorString(err);

    auto errMsg = std::string("CUDA error ") + std::string(errStr);
    
    if (err != cudaError::cudaSuccess) throw errMsg;
}

#endif // USE_CUDA