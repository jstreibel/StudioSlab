//
// Created by joao on 23/09/23.
//

#ifndef STUDIOSLAB_STRING_H
#define STUDIOSLAB_STRING_H

#include <string>
#include <sstream>
#include <vector>

typedef std::string         Str;
typedef std::stringstream   StringStream;
typedef std::istringstream  InStringStream;
typedef std::vector<Str>    StrVector;

namespace StrUtils {
    extern unsigned RealToStringDecimalPlaces;
    extern bool UseScientificNotation;

    void ReplaceLastOccurrence(Str& str, const Str& toReplace, const Str& replaceWith);

    Str ReplaceAll(Str str, const Str& from, const Str& to);

    StrVector Split(const Str& input, const Str& separator);

    StrVector GetLines(const char*);
    StrVector GetLines(const Str&);

}

Str ToStr(const double &a_value);
Str ToStr(const double &a_value, unsigned decimalPlaces, bool useScientificNotation=false);

template <typename T>
Str ToStr(const T &a_value){ return std::to_string(a_value); }
Str ToStr(bool value);
Str ToStr(const Str& str);
Str ToStr(const StrVector& strVector);


#endif //STUDIOSLAB_STRING_H
