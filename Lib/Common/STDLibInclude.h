//
// Created by joao on 17/10/2019.
//

#ifndef V_SHAPE_STDLIBINCLUDE_H
#define V_SHAPE_STDLIBINCLUDE_H


#include "Common/Workaround/ABIDef.h"

#include <cstdarg>
#include <cstdio>
#include <cstdint>

#include <cmath>
//#include <complex>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <variant>
#include <vector>


typedef std::string String;
typedef std::stringstream StringStream;
typedef std::vector<String> StrVector;

namespace Common {
    template<typename T>
    bool contains(const std::vector<T> &vec, const T &val) {
        return std::find(vec.begin(), vec.end(), val) != vec.end();
    }

    StrVector splitString(const String& input, char delimiter);

    template<typename T>
    bool Contains(std::vector<T> vec, T element) {
        return std::find(vec.begin(), vec.end(), element) != vec.end();
    }
}


#if 0 // abaixo uma tentativa que falhou e fica ai para posteridade.
// Se a linguagem for C++17, usamos stdlib. Se nao, usamos boost.
#if __cplusplus == 201703L // ver https://stackoverflow.com/questions/2324658/how-to-determine-the-version-of-the-c-standard-used-by-the-compiler
#include <variant>
typedef std::map<std::string, std::variant<std::string, int, double>> UserParamMap;
#else
#include <boost/variant.hpp>
typedef std::map<std::string, boost::variant<std::string, int, double>> UserParamMap;
#endif
#endif


#endif //V_SHAPE_STDLIBINCLUDE_H
