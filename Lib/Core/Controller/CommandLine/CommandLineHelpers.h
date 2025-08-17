//
// Created by joao on 8/16/25.
//

#ifndef COMMANDLINEHELPERS_H
#define COMMANDLINEHELPERS_H

#include <string>

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
        bool bLowerCase = false;
        char Separator = '-';
    };

    inline std::string SanitizeToValidLongOption(std::string_view s, FLongOptionFormatting Format = {}) {
        std::string out;
        out.reserve(s.size() * 2);

        auto is_alphanum_uc = [](unsigned char c){ return std::isalnum(c) != 0; };

        bool sep_pending = false;

        for (size_t i = 0; i < s.size(); ++i) {
            unsigned char c = static_cast<unsigned char>(s[i]);

            if (is_alphanum_uc(c) || c == '_' || c == '-') {
                // If there were separators before, emit a single '-'
                if (sep_pending) {
                    if (!out.empty() && out.back() != Format.Separator) out.push_back(Format.Separator);
                    sep_pending = false;
                }
                char append = static_cast<char>(c);
                if (Format.bLowerCase) append = static_cast<char>(std::tolower(append));
                out.push_back(append);
            } else {
                // Any other char acts as a separator
                sep_pending = true;
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

        // Ensure first is a letter; if not, prefix "x-"
        if (out.empty() || !std::isalpha(static_cast<unsigned char>(out.front()))) {
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
