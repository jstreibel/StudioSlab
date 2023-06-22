//
// Created by joao on 10/6/21.
//

#include "ByteData.h"

auto operator<< (OStream& stream, const ByteData &byteData) -> OStream&{
    for(auto c : byteData) stream << c;

    return stream;
}