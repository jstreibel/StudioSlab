//
// Created by joao on 08/08/2021.
//

#include "BinarySOF.h"


BinarySOF::BinarySOF() = default;;

auto BinarySOF::getFormatDescription() const -> Str {
    return Str("fp32");
}

auto BinarySOF::operator()(const DiscreteSpace &fOut) const -> ByteData {
    const auto &space = fOut;
    const VecFloat& X = fOut.getHostData(true);

    ByteData data(X.size()*typeSize);

    for(int i=0; i<X.size(); ++i) {
        auto helper = ByteDataConvertHelperUnion((float) X[i]);
        for (int j = 0; j < typeSize; ++j) {
            data[i * typeSize + j] = helper.data[j];
        }
    }

    return data;
}

auto BinarySOF::operator()(const Real &out) const -> ByteData {
    ByteDataConvertHelperUnion<float> byteData((float)out);


    const int size = sizeof(float);
    ByteData data(size);
    for(auto i=0; i<size; ++i) data[i] = byteData.data[i];

    return data;
}

bool BinarySOF::isBinary() const {
    return true;
}
