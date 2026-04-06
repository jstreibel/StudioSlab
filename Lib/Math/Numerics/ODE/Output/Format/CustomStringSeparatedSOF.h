//
// Created by joao on 11/10/2019.
//

#ifndef V_SHAPE_CUSTOMSTRINGSEPARATEDSOF_H
#define V_SHAPE_CUSTOMSTRINGSEPARATEDSOF_H

#include "OutputFormatterBase.h"


namespace Slab::Math {

    class FCustomStringSeparatedSOF : public FOutputFormatterBase {
    public:
        explicit FCustomStringSeparatedSOF(std::string sep = " ");

        auto operator()(const DiscreteSpace &fOut) const -> ByteData override;

        auto operator()(const DevFloat &out) const -> ByteData override;

        auto isBinary() const -> bool override;

        auto getFormatDescription() const -> Str override;

    private:
        const std::string sep;
    };

    using CustomStringSeparatedSOF [[deprecated("Use FCustomStringSeparatedSOF")]] = FCustomStringSeparatedSOF;


}
#endif //V_SHAPE_CUSTOMSTRINGSEPARATEDSOF_H
