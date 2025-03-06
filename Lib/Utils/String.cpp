//
// Created by joao on 23/09/23.
//

#include "String.h"

#include "Types.h"

#define CONVERT_STRINGS_WITH_BOOST_LOCALE true

#if CONVERT_STRINGS_WITH_BOOST_LOCALE==true
#include <boost/locale.hpp>
inline std::string wstring_to_string(const std::wstring& wstr) {
    return boost::locale::conv::utf_to_utf<char>(wstr);
}

inline std::wstring string_to_wstring(const std::string& str) {
    return boost::locale::conv::utf_to_utf<wchar_t>(str);
}
#else
inline std::string wstring_to_string(const std::wstring& wstr) {
    std::mbstate_t state = std::mbstate_t();
        const wchar_t* src = wstr.data();
        size_t len = 1 + std::wcsrtombs(nullptr, &src, 0, &state);

        if (len == static_cast<size_t>(-1)) {
            throw std::runtime_error("Failed to convert wide string to narrow string");
        }

        std::string narrow_string(len, '\0');
        std::wcsrtombs(&narrow_string[0], &src, len, &state);

        return narrow_string;
}

inline std::wstring string_to_wstring(const std::string& str) {
    std::mbstate_t state = std::mbstate_t(); // Initialize conversion state
        const char* src = str.data();            // Source string
        size_t len = 1 + std::mbsrtowcs(nullptr, &src, 0, &state); // Get required length

        if (len == static_cast<size_t>(-1)) {
            throw std::runtime_error("Failed to convert narrow string to wide string");
        }

        std::wstring wide_string(len, L'\0');    // Create wide string buffer
        std::mbsrtowcs(&wide_string[0], &src, len, &state); // Convert

        return wide_string;
}
#endif



namespace Slab {
    unsigned RealToStringDecimalPlaces = 2;
    bool UseScientificNotation = false;

    WStr StrToWStr(const Str& str){
        return string_to_wstring(str);
    }

    Str WStrToStr(const WStr& wstr) {
        return wstring_to_string(wstr);
    }

    void ReplaceLastOccurrence(Str &str, const Str &toReplace, const Str &replaceWith) {
        auto pos = str.rfind(toReplace);
        if (pos != Str::npos) str.replace(pos, toReplace.length(), replaceWith);
    }

    Str ReplaceAll(Str str, const Str &from, const Str &to) {
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

    StrVector Split(const Str &input, const Str &separator) {
        // By Chat GPT

        StrVector tokens;
        size_t start = 0, end = 0;

        while ((end = input.find(separator, start)) != std::string::npos) {
            // Avoid adding empty strings if `sep` occurs at the beginning
            // or more than once in succession.
            if (end != start) {
                tokens.push_back(input.substr(start, end - start));
            }
            start = end + separator.length();
        }

        // Add the last token, if any, to the list.
        if (start != input.length()) {
            tokens.push_back(input.substr(start));
        }

        return tokens;

    }

    bool StrEndsWith(const Str &str, const StrVector &suffixes) {
        for (fix &suffix : suffixes) {
            if (str.ends_with(suffix)) return true;
        }

        return false;
    }

    Str ToStr(const double &a_value, unsigned int decimalPlaces, bool useScientificNotation) {
        auto base = useScientificNotation ? std::scientific : std::fixed;
        std::ostringstream out;
        out.precision(decimalPlaces);
        out << base << a_value;

        return out.str();
    }

    Str ToStr(const double &a_value) {
        return ToStr(a_value, RealToStringDecimalPlaces, UseScientificNotation);
    }

    Str ToStr(bool value) { return value ? "True" : "False"; }

    Str ToStr(const Str &str) {
        // return Str("\"") + str + Str("\"");
        return str;
    }

    Str ToStr(const StrVector &strVector) {
        Str val;

        for (auto &s: strVector) val += s + ";";

        return val;
    }


}