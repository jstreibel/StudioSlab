//
// Created by joao on 11/09/2019.
//

#ifndef V_SHAPE_UTILS_H
#define V_SHAPE_UTILS_H

#include "Types.h"
#include <typeinfo>
#include <cxxabi.h>

#ifdef NDEBUG // Release
#define cast(NAME, TO_TYPE, OBJECT) \
TO_TYPE NAME = static_cast<TO_TYPE>(OBJECT);
#else
#define cast(NAME, TO_TYPE, OBJECT) \
TO_TYPE NAME = dynamic_cast<TO_TYPE>(OBJECT); \
if(&NAME == nullptr) throw "Bad cast.";
#endif

namespace Common {
    template<typename T>
    inline T max(const T &a, const T &b){ return a>b ? a : b; };
    template<typename T>
    inline T min(const T &a, const T &b){ return a<b ? a : b; };

    const Real infty = std::numeric_limits<Real>::infinity();

    double periodic_space(double x, double xMin, double xMax);

    bool areEqual(const Real &lhs, const Real &rhs, Real eps=1.e-5);

    template<class Class>
    Str getClassName(Class *thisClass){
        int status;
        char* demangled_name = abi::__cxa_demangle(typeid(*thisClass).name(), 0, 0, &status);

        Str returnString(demangled_name);

        if(status != 0)
            returnString = "<error demangling name>";

        return returnString;
    }

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

Str ToStr(const double &a_value, const int &decimal_places = 2, bool useScientificNotation= false);

template <typename T>
Str ToStr(const T &a_value){ return std::to_string(a_value); }
Str ToStr(bool value);
Str ToStr(const Str& str);



#endif //V_SHAPE_UTILS_H
