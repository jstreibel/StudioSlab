//
// Created by joao on 08/08/2021.
//

#ifndef V_SHAPE_BINARYSOF_H
#define V_SHAPE_BINARYSOF_H


#include "OutputFormatterBase.h"


namespace Slab::Math {

    class FBinarySOF : public FOutputFormatterBase {
    public:
        const int typeSize = sizeof(float);

        explicit FBinarySOF();

        auto operator()(const DevFloat &out) const -> ByteData override;

        auto operator()(const DiscreteSpace &fOut) const -> ByteData override;

        auto isBinary() const -> bool override;

        auto getFormatDescription() const -> Str override;

    private:
        const std::string sep;
    };

    using BinarySOF [[deprecated("Use FBinarySOF")]] = FBinarySOF;


}

#endif //V_SHAPE_BINARYSOF_H
