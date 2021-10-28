//
// Created by joao on 10/6/21.
//

#ifndef FIELDS_BYTEDATA_H
#define FIELDS_BYTEDATA_H


#include <vector>
#include <ostream>

typedef std::vector<char> ByteData;

template<typename TYPE>
union ByteDataConvertHelperUnion {
    explicit ByteDataConvertHelperUnion(TYPE value) : value(value) { }

    char data[sizeof(TYPE)];
    TYPE value;
};

typedef std::vector<char> ByteData;

auto operator<< (std::ostream& stream, const ByteData &byteData) -> std::ostream&;

#endif //FIELDS_BYTEDATA_H
