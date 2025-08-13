//
// Created by joao on 13/09/2019.
//

#include "Utils.h"
#include <cstdarg>
#include <cmath>

#include <bitset>
#include <algorithm>
#include <string>
#include <vector>
#include <filesystem>

namespace Slab::Common {

    void PrintThere(int x, int y, const char *format, ...) {
        va_list args;
        va_start(args, format);
        printf("\033[%d;%dH", x, y);
        vprintf(format, args);
        va_end(args);
        fflush(stdout);
    }

    char GetDensityChar(float dens, bool useLongSeq) {
        static const Str shortSeq = Str(".:-=+*#%@");
        static const Str longSeq = Str(R"($@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,"^`'. )");

        Str seq = useLongSeq ? longSeq : shortSeq;

        int N = (int) seq.length(); // leave as int!!! Negative values required
        fix ascii_gs = seq.c_str();

        int loc = N - int(dens * (float) N);
        loc = loc > N - 1 ? N - 1 : loc < 0 ? 0 : loc;

        return ascii_gs[loc];
    }

    void PrintDensityThere(int x, int y, float dens) {
        PrintThere(x, y, "%c", GetDensityChar(dens));
    }


    Str Replace(Str str, char oldChar, char newChar) {
        std::replace(str.begin(), str.end(), oldChar, newChar);

        return str;
    }



    StrVector SplitString(const Str &s, const Str &delimiter, unsigned int maxTokens) {
        StrVector tokens;
        size_t start = 0;
        size_t end = s.find(delimiter);
        int tokenCount = 0;

        while (end != std::string::npos) {
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

    unsigned BinaryToUInt(std::string binary, char zero, char one) {
        if (zero != '0') binary = Replace(binary, zero, '0');
        if (one != '1') binary = Replace(binary, one, '1');

        std::bitset<32> x(binary);

        return x.to_ullong();
    }

    unsigned short BinaryToUShort(std::string binary, char zero, char one) {
        if (zero != '0') binary = Replace(binary, zero, '0');
        if (one != '1') binary = Replace(binary, one, '1');

        std::bitset<16> x(binary);

        return x.to_ulong();
    }

    double periodic_space(double x, double xMin, double xMax) {
        fix Δx = xMax - xMin;
        return xMin + std::fmod(x - xMin, Δx);
    }

    bool AreEqual(const DevFloat &lhs, const DevFloat &rhs, DevFloat eps) {
        return std::abs(lhs - rhs) < eps;
    }



    Str GetPWD() { return std::filesystem::current_path(); }

    /*
    double RoundToMostSignificantDigits(double num, int digits) {
        if (num == 0.0) return 0.0;

        double exponent = floor(log10(fabs(num)));
        double scale = pow(10.0, -exponent);

        double scaledNum = num * scale;

        return round(scaledNum * pow(10.0, digits)) / pow(10.0, digits) / scale;
    }
    */
    double RoundToMostSignificantDigits(const double num, const int digits) {
        const double factor = std::pow(10.0, digits);
        return std::round(num * factor) / factor;
    }

}