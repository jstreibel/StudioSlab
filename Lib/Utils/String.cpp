//
// Created by joao on 23/09/23.
//

#include "String.h"

namespace StrUtils {
    unsigned RealToStringDecimalPlaces = 2;
    bool UseScientificNotation = false;

}
Str ToStr(const double &a_value, unsigned int decimalPlaces, bool useScientificNotation) {
    auto base = useScientificNotation ? std::scientific : std::fixed;
    std::ostringstream out;
    out.precision(decimalPlaces);
    out << base << a_value;

    return out.str();
}

Str ToStr(const double &a_value) {
    return ToStr(a_value, StrUtils::RealToStringDecimalPlaces, StrUtils::UseScientificNotation);
}

Str ToStr(bool value)       { return value ? "True" : "False"; }

Str ToStr(const Str &str)   {
    // return Str("\"") + str + Str("\"");
    return str;
}

