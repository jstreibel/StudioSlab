//
// Created by joao on 23/09/23.
//

#ifndef STUDIOSLAB_STRING_H
#define STUDIOSLAB_STRING_H

#include <string>
#include <sstream>

typedef std::string         Str;
typedef std::stringstream   StringStream;

namespace StrUtils {
    extern unsigned RealToStringDecimalPlaces;
    extern bool UseScientificNotation;
}

Str ToStr(const double &a_value);
Str ToStr(const double &a_value, unsigned decimalPlaces, bool useScientificNotation=false);

template <typename T>
Str ToStr(const T &a_value){ return std::to_string(a_value); }
Str ToStr(bool value);
Str ToStr(const Str& str);


#endif //STUDIOSLAB_STRING_H
