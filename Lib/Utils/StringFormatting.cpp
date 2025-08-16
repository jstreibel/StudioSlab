//
// Created by joao on 8/16/25.
//

#include "StringFormatting.h"

#include <cctype>

namespace Slab
{
    Str ToSnakeCase(std::string_view s)
    {
        std::string out;
        out.reserve(s.size() * 2);

        enum class Prev { None, Lower, Upper, Digit, Underscore };
        Prev prev = Prev::None;
        bool sep_pending = false;

        auto is_alnum_uc = [](unsigned char c){ return std::isalnum(c) != 0; };
        auto is_upper_uc = [](unsigned char c){ return std::isupper(c) != 0; };
        auto is_lower_uc = [](unsigned char c){ return std::islower(c) != 0; };

        for (size_t i = 0; i < s.size(); ++i) {
            unsigned char c = static_cast<unsigned char>(s[i]);

            if (is_alnum_uc(c)) {
                // Determine if we need an underscore before this char
                bool need_us = false;

                // Case 1: pending separator(s) behind us
                if (sep_pending && !out.empty() && out.back() != '_') {
                    need_us = true;
                }

                // Case 2: lower|digit -> UPPER boundary
                if (!need_us && is_upper_uc(c) &&
                    (prev == Prev::Lower || prev == Prev::Digit)) {
                    need_us = true;
                    }

                // Case 3: ACRONYM boundary: ...[A][B][c] -> insert '_' before B
                // That is: prev was UPPER, current is UPPER, next is lower
                if (!need_us && prev == Prev::Upper && is_upper_uc(c)) {
                    if (i + 1 < s.size() && is_lower_uc(static_cast<unsigned char>(s[i+1]))) {
                        need_us = true;
                    }
                }

                if (need_us) out.push_back('_');

                // Append lowercased alnum
                out.push_back(static_cast<char>(std::tolower(c)));

                // Update prev class
                if (is_lower_uc(c))      prev = Prev::Lower;
                else if (is_upper_uc(c)) prev = Prev::Upper;
                else                     prev = Prev::Digit;

                sep_pending = false;
            } else {
                // Any non-alnum is a separator
                sep_pending = true;
                prev = Prev::Underscore;
            }
        }

        // Trim trailing underscore if introduced by trailing separators
        if (!out.empty() && out.back() == '_') {
            out.pop_back();
        }

        return out;
    }
}