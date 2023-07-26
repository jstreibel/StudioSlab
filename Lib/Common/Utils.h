//
// Created by joao on 11/09/2019.
//

#ifndef V_SHAPE_UTILS_H
#define V_SHAPE_UTILS_H

#include "Types.h"

#ifdef NDEBUG // Release
#define cast(NAME, TO_TYPE, OBJECT) \
TO_TYPE NAME = static_cast<TO_TYPE>(OBJECT);
#else
#define cast(NAME, TO_TYPE, OBJECT) \
TO_TYPE NAME = dynamic_cast<TO_TYPE>(OBJECT); \
if(&NAME == nullptr) throw "Bad cast.";
#endif


#define let auto
#define letc const auto
#define letcp const auto*
#define letr auto&
#define letcr const auto&
#define fix const auto
//#define pt auto *
#define look const auto *


namespace Common {

    void                    PrintThere(int x, int y, const char *format, ...);
    auto                    GetDensityChar(float dens) -> char;
    void                    PrintDensityThere(int x, int y, float dens);

    template<typename T>
    bool                    contains(const std::vector<T> &vec, const T &val) {
        return std::find(vec.begin(), vec.end(), val) != vec.end();
    }
    template<typename T>
    bool                    Contains(std::vector<T> vec, const T &element) {
        return std::find(vec.begin(), vec.end(), element) != vec.end();
    }
    template<typename T>
    bool                    Contains(std::set<T> vec, const T &element) {
        return std::find(vec.begin(), vec.end(), element) != vec.end();
    }

    /**
     * Splits a string in 'maxTokens' tokens, separated by 'delimiter', starting from beginning of string.
     * @param s The string to be separated in tokens.
     * @param delimiter The delimiter to use.
     * @param maxTokens The maximum number of tokens to separate the string. Negative values mean no limit.
     * @return A vector containing all the separated strings.
     */
    StrVector               SplitString(const Str& s, const Str &delimiter, unsigned int maxTokens=(unsigned)-1);

    unsigned                BinaryToUInt(std::string binary, char zero='0', char one='1');
    unsigned short          BinaryToUShort(std::string binary, char zero='0', char one='1');

}


#if USE_CUDA

#include <driver_types.h>

// cw stands for cuda error wrapper
void cew(cudaError err);

#endif // USE_CUDA

template <typename T>
Str ToStr(const T a_value, const int n = 2, bool useScientificNotation= false)
{
    auto base = useScientificNotation ? std::scientific : std::fixed;
    std::ostringstream out;
    out.precision(n);
    out << base << a_value;

    return out.str();
}


#endif //V_SHAPE_UTILS_H
