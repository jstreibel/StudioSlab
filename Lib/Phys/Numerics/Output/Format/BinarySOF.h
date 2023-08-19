//
// Created by joao on 08/08/2021.
//

#ifndef V_SHAPE_BINARYSOF_H
#define V_SHAPE_BINARYSOF_H


#include "OutputFormatterBase.h"


class BinarySOF : public OutputFormatterBase {
public:
    const int typeSize = sizeof(float);

    explicit BinarySOF();

    auto operator()(const Real &out) const -> Numerics::ByteData override;

    auto operator()(const DiscreteSpace &fOut) const -> Numerics::ByteData override;

    auto isBinary() const -> bool override;

    auto getFormatDescription() const -> Str override;

private:
    const std::string sep;
};


#endif //V_SHAPE_BINARYSOF_H
