//
// Created by joao on 8/16/25.
//

#ifndef STRING_FORMATTING_H
#define STRING_FORMATTING_H

#include <string>
#include <string_view>
#include "String.h"

namespace Slab {
    auto ToSnakeCase(std::string_view s) -> Str;
} // namespace util


#endif //STRING_FORMATTING_H
