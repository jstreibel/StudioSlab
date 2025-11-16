//
// Created by joao on 8/16/25.
//

#ifndef COMMANDLINEHELPERS_H
#define COMMANDLINEHELPERS_H

#include <string>
#include <string_view>
#include <cctype>

namespace Slab::Core
{
    inline bool IsValidCLIArg(const std::string& arg) {
        if (arg.empty()) return false;

        // First character: must be a letter
        if (!std::isalpha(static_cast<unsigned char>(arg[0]))) {
            return false;
        }

        for (char c : arg) {
            if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_'))
            {
                return false;
            }
        }

        return true;
    }

    struct FLongOptionFormatting
    {
        bool bLowerCase = true;   // default to lowercase output
        char Separator = '-';
    };

    inline std::string SanitizeToValidLongOption(std::string_view s, FLongOptionFormatting Format = {}) {
        std::string out;
        out.reserve(s.size() * 2);

        auto is_alphanum_uc = [](unsigned char c){ return std::isalnum(c) != 0; };
        auto is_non_ascii   = [](unsigned char c){ return c >= 128; };
        auto is_sep_char    = [](unsigned char c){ return std::isspace(c) || c=='.' || c==','; };

        bool sep_pending = false;
        bool dropped_disallowed = false; // track if we drop any truly disallowed chars

        for (size_t i = 0; i < s.size(); ++i) {
            unsigned char c = static_cast<unsigned char>(s[i]);

            // Treat '-' as a separator to collapse multiple dashes
            if (is_alphanum_uc(c) || c == '_') {
                if (sep_pending) {
                    if (!out.empty() && out.back() != Format.Separator) out.push_back(Format.Separator);
                    sep_pending = false;
                }
                char append = static_cast<char>(c);
                if (Format.bLowerCase) append = static_cast<char>(std::tolower(append));
                out.push_back(append);
            } else if (c == '-' || is_sep_char(c) || is_non_ascii(c)) {
                // Map known separators and non-ASCII bytes to a single separator (collapsed)
                sep_pending = true;
            } else {
                // Drop disallowed punctuation (e.g., $, ", etc.) but remember we dropped
                dropped_disallowed = true;
            }
        }

        // Trim trailing separators
        while (!out.empty() && (out.back() == '-' || out.back() == '_')) {
            out.pop_back();
        }
        // Trim leading separators
        size_t start = 0;
        while (start < out.size() && (out[start] == '-' || out[start] == '_')) {
            ++start;
        }
        if (start > 0) out.erase(0, start);

        // Ensure first is a letter; if not, prefix "x-". Also prefix if we had to drop
        // any disallowed punctuation (to make provenance explicit as in tests).
        if (out.empty()) {
            out = "x";
        } else if (!std::isalpha(static_cast<unsigned char>(out.front())) || dropped_disallowed) {
            // Only add the dash if resulting first is a letter/digit so we get "x-foo".
            if (!out.empty() && out.front() != '-') {
                out.insert(out.begin(), {'x','-'});
            } else {
                out = "x";
            }
        }

        return out;
    }


}

#endif //COMMANDLINEHELPERS_H
