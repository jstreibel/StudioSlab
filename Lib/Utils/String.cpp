//
// Created by joao on 23/09/23.
//

#include "String.h"

namespace StrUtils {
    unsigned RealToStringDecimalPlaces = 2;
    bool UseScientificNotation = false;

    void ReplaceLastOccurrence(Str& str, const Str& toReplace, const Str& replaceWith) {
        auto pos = str.rfind(toReplace);
        if (pos != Str::npos)  str.replace(pos, toReplace.length(), replaceWith);
    }

    Str ReplaceAll(Str str, const Str& from, const Str& to) {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
        }

        return str;
    }

    StrVector GetLines(const char *text) {
        StrVector lines;
        InStringStream stream(text);
        Str line;

        while (std::getline(stream, line))
            lines.push_back(line);

        return lines;
    }

    StrVector GetLines(const Str &text) {
        return GetLines(text.c_str());
    }
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

Str ToStr(const StrVector& strVector){
    Str val;

    for(auto &s : strVector) val += s + ";";

    return val;
}


