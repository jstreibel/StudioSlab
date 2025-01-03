//
// Created by joao on 10/6/21.
//

#ifndef FIELDS_BYTEDATA_H
#define FIELDS_BYTEDATA_H

#include "Utils/Types.h"

namespace Slab::Math {
    typedef Vector<char> ByteData;

    template<typename TYPE>
    union ByteDataConvertHelperUnion {
        explicit ByteDataConvertHelperUnion(TYPE value) : value(value) {}

        char data[sizeof(TYPE)];
        TYPE value;
    };


    auto operator<<(OStream &stream, const ByteData &byteData) -> OStream &;

}

#endif //FIELDS_BYTEDATA_H
