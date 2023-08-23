//
// Created by joao on 13/09/2019.
//

#include "Utils.h"

#include <cmath>

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

#include <bitset>
#include <algorithm>

Str Replace(std::string str, char oldChar, char newChar) {
    std::replace(str.begin(), str.end(), oldChar, newChar);

    return str;
}

#include <string>
#include <vector>

StrVector Common::SplitString(const Str &s, const Str &delimiter, unsigned int maxTokens)
{
    StrVector tokens;
    size_t start = 0;
    size_t end = s.find(delimiter);
    int tokenCount = 0;

    while (end != std::string::npos)
    {
        tokens.push_back(s.substr(start, end - start));
        tokenCount++;

        if (tokenCount >= maxTokens - 1) {
            start = end + delimiter.length();
            auto lastToken = s.substr(start, std::string::npos);
            tokens.push_back(lastToken);
            break;
        }

        start = end + delimiter.length();
        end = s.find(delimiter, start);
    }

    if (tokens.size() < maxTokens) {
        tokens.push_back(s.substr(start, std::string::npos));
    }

    return tokens;
}

/*
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
}*/

unsigned Common::BinaryToUInt(std::string binary, char zero, char one) {
    if(zero != '0') binary = Replace(binary, zero, '0');
    if(one  != '1') binary = Replace(binary, one, '1');

    std::bitset<32>  x(binary);

    return x.to_ullong();
}

unsigned short Common::BinaryToUShort(std::string binary, char zero, char one){
    if(zero != '0') binary = Replace(binary, zero, '0');
    if(one  != '1') binary = Replace(binary, one, '1');

    std::bitset<16>  x(binary);

    return x.to_ulong();
}

double Common::periodic_space(double x, double xMin, double xMax) {
    fix Δx = xMax - xMin;
    return xMin + std::fmod(x - xMin, Δx);
}

bool Common::areEqual(const Real &lhs, const Real &rhs, const Real eps) {
    return std::abs(lhs-rhs) < eps;
}

Str ToStr(const double &a_value, const int &decimal_places, bool useScientificNotation) {
    auto base = useScientificNotation ? std::scientific : std::fixed;
    std::ostringstream out;
    out.precision(decimal_places);
    out << base << a_value;

    return out.str();
}

Str ToStr(bool value)       { return value ? "True" : "False"; }

Str ToStr(const Str &str)   { return Str("\"") + str + Str("\""); }



