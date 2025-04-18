//
// Created by joao on 11/09/2019.
//

#ifndef V_SHAPE_UTILS_H
#define V_SHAPE_UTILS_H

#include "Types.h"
#include <typeinfo>
#include <cxxabi.h>
#include <set>

#ifdef NDEBUG // Release
#define SlabCast(NAME, TO_TYPE, OBJECT) \
TO_TYPE NAME = static_cast<TO_TYPE>(OBJECT);
#else
#define SlabCast(NAME, TO_TYPE, OBJECT) \
TO_TYPE NAME = dynamic_cast<TO_TYPE>(OBJECT); \
if(&NAME == nullptr) throw Exception("Bad cast.");
#endif

#if USE_CUDA

#include <driver_types.h>

namespace Slab::CUDA {
// cew stands for Cuda Error Wrapper
void cew(cudaError err);
}

#endif // USE_CUDA

#include "String.h"
#include "Containers.h"

namespace Slab::Common {
    double RoundToMostSignificantDigits(double num, int digits = 3);

    Str GetPWD();

    template<typename T>
    inline T max(const T &a, const T &b) { return a > b ? a : b; };

    template<typename T>
    inline T min(const T &a, const T &b) { return a < b ? a : b; };

    const Real infty = std::numeric_limits<Real>::infinity();

    double periodic_space(double x, double xMin, double xMax);

    bool AreEqual(const Real &lhs, const Real &rhs, Real eps = 1.e-5);

    template<typename T>
    bool AreEqual(const T &lhs, const T &rhs) {
        return lhs==rhs;
    }

    template<class Class>
    Str getClassName(Class *thisClass) {
        int status;
        char *demangled_name = abi::__cxa_demangle(typeid(*thisClass).name(), 0, 0, &status);

        Str returnString(demangled_name);

        if (status != 0)
            returnString = "<error demangling name>";

        return returnString;
    }

    void PrintThere(int x, int y, const char *format, ...);

    auto GetDensityChar(float dens, bool longSeq = false) -> char;

    void PrintDensityThere(int x, int y, float dens);


    // template<typename T>
    // bool                    Contains(Vector<T> vec, const T &element) {
    //     return std::find(vec.begin(), vec.end(), element) != vec.end();
    // }
    // template<typename T>
    // bool                    Contains(std::set<T> set, const T &element) {
    //     return std::find(set.begin(), set.end(), element) != set.end();
    // }

    /**
     * Splits a string in 'maxTokens' tokens, separated by 'delimiter', starting from beginning of string.
     * @param s The string to be separated in tokens.
     * @param delimiter The delimiter to use.
     * @param maxTokens The maximum number of tokens to separate the string. Negative values mean no limit.
     * @return A vector containing all the separated strings.
     */
    StrVector SplitString(const Str &s, const Str &delimiter, unsigned int maxTokens = (unsigned) -1);

    unsigned BinaryToUInt(std::string binary, char zero = '0', char one = '1');

    unsigned short BinaryToUShort(std::string binary, char zero = '0', char one = '1');

}

#endif //V_SHAPE_UTILS_H
