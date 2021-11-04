//
// Created by joao on 10/6/21.
//

#include "ByteData.h"

auto operator<< (std::ostream& stream, const ByteData &byteData) -> std::ostream&{
    for(auto c : byteData) stream << c;

    return stream;
}