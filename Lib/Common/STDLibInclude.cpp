//
// Created by joao on 28/05/23.
//

#include "STDLibInclude.h"

StrVector Common::splitString(const String &input, char delimiter) {
    StrVector tokens;
    std::size_t start = 0;
    std::size_t end = input.find(delimiter);

    while (end != String::npos) {
        std::string token = input.substr(start, end - start);
        tokens.push_back(token);
        start = end + 1;
        end = input.find(delimiter, start);
    }

    // Add the last token (or the only token if no delimiter is found)
    std::string lastToken = input.substr(start);
    tokens.push_back(lastToken);

    return tokens;
}